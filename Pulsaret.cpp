#include "Pulsaret.h"

void Pulsaret::setup(float sampleRate, 
					const vector<float>& oscTable, 
					const vector<float>& windowTable, 
					const vector<vector<float>>& panTables) {
	
	// setup oscillator & window
	osc_.setSampleRate(sampleRate);
	window_.setSampleRate(sampleRate);
	osc_.setTable(oscTable);
	window_.setTable(windowTable);
	
	// setup panning wavetables
	for(int chan = 0; chan < NUM_OUTPUTS; ++chan) {
		panning_[chan].setTable(panTables[chan]);
	}
}

void Pulsaret::trigger(float oscHz, float windowHz, float panPos, float level) {
	
	for(auto& chan : panning_) {
		chan.setPanPos(panPos);
	}
	
	// set oscillator frequency & window duration
	osc_.setHz(oscHz);
	window_.setHz(windowHz);
	
	// trigger oscillator and window function
	osc_.trigger();
	window_.trigger(level);
}

void Pulsaret::process(float (&outputs)[NUM_OUTPUTS]) {
	
	if(!window_.isActive()) {
		std::fill(outputs, outputs + NUM_OUTPUTS, 0.f);
		return;
	}
	
	// apply window function to oscillator
	float out = window_.process() * osc_.process();
	
	for(int chan = 0; chan < NUM_OUTPUTS; ++chan) {
		outputs[chan] = out * panning_[chan].process();
	}
}