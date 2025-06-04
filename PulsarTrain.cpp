#include "PulsarTrain.h"
#include <cstdlib>
#include <libraries/math_neon/math_neon.h>
#include "Utils.h"

float PulsarTrain::dBRange = std::abs(MIN_DB);

void PulsarTrain::setup(float sampleRate, 
						const vector<float>& oscTable, 
						const vector<float>& windowTable, 
						const vector<vector<float>>& panTables,
						bool dcBlock) {
	
	// setup pulsarets						
	for(auto& pulsaret : pulsarPool_) {
		pulsaret.setup(sampleRate, oscTable, windowTable, panTables);
	}
	
	// are we using DC blocking?
	dcBlockActive_ = dcBlock;
	
	// default oscillator frequency
	oscHz_ = 440.f;
	
	// default window duration
	windowHz_ = 20.f;
	
	level_ = 1.f;
	
	// default burst masking values
	burstMaskActive_ = false;
	burstInvert_ = false;
	burstOn_ = 8;
	burstTotal_ = 16;
	burstCounter_ = 0;
	
	// default stochastic masking values
	stochMaskActive_ = false;
	stochProb_ = 1.f;
	
	// default pan position
	panPos_ = .5f;
	
	// pan variation off by default
	panVari_ = 0.f;
	
	// default channel masking values
	chanMask_ = false;
	chanInvert_ = false;
	chanOn_ = 8;
	chanTotal_ = 16;
	chanCounter_ = 0;
	chanWidth_ = .5f;
	chanCountMode_ = UNBURST;
	
	// seed pseudo-random number generator
	srand(time(nullptr));
	
	// precalculate reciprocal of maximum random value
	inverseRandMax_ = 1.f / (float)RAND_MAX;
}

void PulsarTrain::setBurstRatio(int on, int off, bool invert) {
	
	// setting a burst ratio switches on burst masking
	burstMaskActive_ = true;
	
	burstOn_ = on;
	
	// calculate total of ratio
	burstTotal_ = on + off;
	
	// reset counting
	burstCounter_ = 0;
	
	burstInvert_ = invert;
}

bool PulsarTrain::burstMask() {
	
	// when burst masking is inactive, never mask
	if(!burstMaskActive_) return false;
	
	// mask when in off part of ratio
	bool mask = burstCounter_ >= burstOn_;
	
	// when we reach the total of the ratio
	if(++burstCounter_ >= burstTotal_) {
		
		// when counting channel masking from burst cycles, increment
		if(chanCountMode_ == BURST_CYCLE) {
			if(++chanCounter_ >= chanTotal_) chanCounter_ = 0;
		}
		
		// reset counter 
		burstCounter_ = 0;
	}
	
	// if burst invert is active, invert result
	return burstInvert_ ? !mask : mask;
}

void PulsarTrain::setStochProb(float value) {
	
	// limit value between 0 & 1
	stochProb_ = utils::clamp(value);
	
	// setting a probabilty automatically engages stochastic masking 
	stochMaskActive_ = true;
}

bool PulsarTrain::stochMask() {
	
	// when stochastic masking is active & probabilty 
	// is less than random value, apply masking
	if(stochMaskActive_ && (stochProb_ < randValue())) return true;
	return false;
}

void PulsarTrain::setChanRatio(int on, int off, bool invert) {
	
	// setting a ratio automatically engages channel masking 
	chanMask_ = true;
	
	chanOn_ = on;
	
	// calculate total of ratio
	chanTotal_ = on + off;
	
	// reset counter 
	chanCounter_ = 0;
	
	// are we inverting the order of channels?
	chanInvert_ = invert;
}

void PulsarTrain::setChanMaskMode(chanMaskMode mode) {
	
	// set the channel masking counting mode
	chanCountMode_ = mode;
	
	// only allow unburst counting mode when burst masking is active
	if(chanCountMode_ == BURST_CYCLE && !burstMaskActive_) {
		chanCountMode_ = UNBURST;
	}
}

void PulsarTrain::setChanWidth(float width) {
	
	// limit value between 0 & 1
	width = utils::clamp(width);
	
	// since we are going to use in a unipolar range, scale by half
	chanWidth_ = width * .5f;
}

float PulsarTrain::randValue(float min, float max) {
	
	// return a random value between min & max
	return min + (max - min) * (float)rand() * inverseRandMax_;
}

void PulsarTrain::trigger() {
	
	// are we masking a pulsaret due to burst masking?
	bool burstMasked = burstMask();
	
	// when we're counting channel masking by each trigger 
	// (before burst masking is applied), increment counter
	if(chanCountMode_ == TRIGGER) {
		if(++chanCounter_ >= chanTotal_) chanCounter_ = 0;
	}
	
	// when we're counting channel masking by each trigger
	// (after burst masking is applied), increment counter
	else if(chanCountMode_ == UNBURST && !burstMasked) {
		if(++chanCounter_ >= chanTotal_) chanCounter_ = 0;
	}
	
	// when pulsaret isn't masked by burst or stochastic masking
	if(!burstMasked && !stochMask()) {
	
		// find 1st free pulsaret	
		for(int i = 0; i < MAX_OVERLAP; ++i) {
			if(!pulsarPool_[i].isActive()) {
				
				float pan = panPos_;
				
				if(chanMask_) {
					bool leftChan = (chanCounter_ < chanOn_);
					if(chanInvert_) leftChan = !leftChan;
					pan += leftChan ? -chanWidth_ : chanWidth_;
				}
				
				// add random pan variation to pan position
				pan += randValue(-panVari_, panVari_);
				
				// limit pan position between 0 & 1
				pan = utils::clamp(pan);
				
				// when level is 0, pulsar train should be silent
				float amp = 0.f;
				if(level_ > 0.f) {
					
					float dB = MIN_DB + (level_ * dBRange);
					amp = powf_neon(10.f, dB / 20.f);
				}
				
				// trigger pulsaret
				pulsarPool_[i].trigger(oscHz_, windowHz_, pan, amp);
				
				// pulsaret found, no need to keep looking
				break;
			}
		}
	}
}

void PulsarTrain::setPanPos(float value) {
	
	// limit value between 0 & 1
	panPos_ = utils::clamp(value);
}

void PulsarTrain::setPanVari(float value) {
	
	// limit value between 0 & 1
	panVari_ = utils::clamp(value);
	
	// since we are going to use in a unipolar range, scale by half
	panVari_ *= .5f;
}

void PulsarTrain::setGain(float value) {
	
	// limit value between 0 & 1
	level_ = utils::clamp(value);
}

void PulsarTrain::process(float (&outputs)[NUM_OUTPUTS]) {
	
	float tempOuts[NUM_OUTPUTS] = {0.f, 0.f};
	for(int i = 0; i < MAX_OVERLAP; ++i) {
		
		// get output of pulsarets
		float pulsaretOut[NUM_OUTPUTS];
		pulsarPool_[i].process(pulsaretOut);
		
		// add pulsaret outputs to array
		for(int chan = 0; chan < NUM_OUTPUTS; ++chan) {
			tempOuts[chan] += pulsaretOut[chan];
		}
	}
	
	for(int chan = 0; chan < 2; ++chan) {
		
		// when dc blocking is active, apply it
		if(dcBlockActive_) {
			tempOuts[chan] = dcBlock_[chan].process(tempOuts[chan]);
		}
		
		// add this pulsar train to referenced array
		outputs[chan] += tempOuts[chan];
	}
}