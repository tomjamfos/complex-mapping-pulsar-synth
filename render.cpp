/*
	To do:
		- how can the ratios (burst, channel) be set from the inputs?
		- add control of pan variation from inputs
*/
#include <Bela.h>
#include <vector>
#include <libraries/math_neon/math_neon.h>
#include <libraries/OnePole/OnePole.h>
#include "Debouncer.h"
#include "PushButton.h"
#include "TouchBar.h"
#include "TouchSquare.h"
#include "Clock.h"
#include "Tables.h"
#include "PulsarTrain.h"
#include "Butterworth4.h"
#include "Utils.h"
#include "ParamWeights.h"
using std::vector;

#define DEBUG
#ifdef DEBUG
#include <libraries/Scope/Scope.h>
Scope scope;
#endif

constexpr int NUM_BARS = 4;
constexpr int AUDIO_OUTS = 2;

// auxiliary task 
AuxiliaryTask readTask;
constexpr float READ_HZ = 80.f;
unsigned readSamps;

// latch switches
constexpr int LATCH_PINS[NUM_BARS] = {8, 9, 11, 12};
Debouncer latchDebouncers[NUM_BARS];

enum RecordState {OFF, RECORD, LOOP};
constexpr float MAX_REC_SECS = 12.f;
unsigned maxRecSamps;

// record button
constexpr int REC_BUTTON_PIN = 3;
constexpr int REC_LED_PIN = 1;
PushButton recButton;
constexpr float LED_BLINK_HZ = 2.5f;
unsigned ledBlinkSamps;

// loop button
constexpr int LOOP_BUTTON_PIN = 4;
constexpr int LOOP_LED_PIN = 2;
PushButton loopButton;

// Trill sensors
TouchBar bars[NUM_BARS];
TouchSquare square;

// bar LEDs
constexpr int BAR_LED_PINS[NUM_BARS] = {7, 5, 4, 3};
constexpr float LED_GAMMA = 2.65f;
constexpr float INVERSE_LED_GAMMA = 1.f / LED_GAMMA;

// master volume pot
constexpr int MAST_OUT_PIN = 0;
OnePole mastOutputSmooth;

int analogPerAudioSamps;

// pulsar trains
constexpr int NUM_TRAINS = 3;
constexpr float PULSE_HZ_BASE = 2.f;
constexpr float PULSE_OCTAVES = 7.f;
constexpr float WIN_RATIO_MIN = .333f;
constexpr float WIN_RATIO_MAX = 32.f;
constexpr float OSC_HZ_BASE = 55.f;
constexpr float OSC_OCTAVES = 3.f;
Clock pulse;
PulsarTrain trains[NUM_TRAINS];
vector<float> oscTable;
vector<float> windowTables[NUM_TRAINS];
vector<vector<float>> panTables; 

// anti-aliasing
Butterworth4 antiAlias[2];
constexpr bool antiAliasing = true;

void readI2C(void*) {
	
	// read sensors 
	for(auto& bar : bars) bar.read();
	square.read();
}

bool setup(BelaContext *context, void *userData) {
	
	// convert auxiliary task read rate from Hz to samples
	readSamps = (int)(context->audioSampleRate / READ_HZ);
	
	// setup auxiliary task
	constexpr int auxTaskPriority = 70;
	readTask = Bela_createAuxiliaryTask(readI2C, auxTaskPriority, "Read I2C");
	
	constexpr float debounceMs = 65.f;
	
	// setup record button & LED
	pinMode(context, 0, REC_BUTTON_PIN, INPUT);
	pinMode(context, 0, REC_LED_PIN, OUTPUT);
	recButton.setup(context->audioSampleRate, debounceMs);
	
	// convert maximum record time from seconds to samples
	maxRecSamps = (int)(context->audioSampleRate * MAX_REC_SECS);
	
	// setup loop button & LED
	pinMode(context, 0, LOOP_BUTTON_PIN, INPUT);
	pinMode(context, 0, LOOP_LED_PIN, OUTPUT);
	loopButton.setup(context->audioSampleRate, debounceMs);
	
	constexpr uint8_t barAddresses[NUM_BARS] = {0x20, 0x21, 0x22, 0x23};
	
	for(int i = 0; i < NUM_BARS; ++i) {
		
		// setup Trill Bars
		if(!bars[i].setup(barAddresses[i], TouchBar::RIGHT, true, readSamps)) {
			fprintf(stderr, "Unable to locate Trill Bar at address 0x%02x\n", barAddresses[i]);
			return false;
		}
		bars[i].setRecTime(MAX_REC_SECS, READ_HZ);
		
		// setup latch switches
		pinMode(context, 0, LATCH_PINS[i], INPUT);
		latchDebouncers[i].setup(context->audioSampleRate, debounceMs);
	}
	
	// setup Trill square
	constexpr uint8_t squareAddress = 0x28;
	if(!square.setup(squareAddress, TouchSquare::RIGHT, true, readSamps)) {
		fprintf(stderr, "Unable to locate Trill Square at address 0x%02x\n", squareAddress);
		return false;
	}
	
	// calculate number of analog samples per audio sample
	analogPerAudioSamps = (int)(context->audioSampleRate / context->analogSampleRate);
	
	// convert record led blink rate from Hz to samples
	ledBlinkSamps = (int)(context->audioSampleRate / LED_BLINK_HZ);
	
	// setup clock
	pulse.setSampleRate(context->audioSampleRate);
	
	// setup wavetables
	oscTable = waveforms::triangle();
	windowTables[0] = windows::hann();
	windowTables[1] = windows::gaussian();
	windowTables[2] = windows::expDecay();
	panTables = pan::constantPower();
	
	// setup trains
	for(int i = 0; i < NUM_TRAINS; ++i) {
		trains[i].setup(context->audioSampleRate, oscTable, windowTables[i], panTables);
	}
	
	trains[1].setBurstRatio(21, 7, true);
	trains[1].setChanRatio(8, 8);
	trains[1].setChanMaskMode(PulsarTrain::BURST_CYCLE);
	trains[1].setChanWidth(.65f);
	
	trains[2].setBurstRatio(21, 7);
	trains[2].setPanVari(.25f);
	
	// setup anti-aliasing filters
	if(antiAliasing) {
		for(auto& chan : antiAlias) chan.setup(context->audioSampleRate);
	}
	
	// setup master volume pot smoothing filter
	mastOutputSmooth.setup(6.f, context->analogSampleRate);
	
	#ifdef DEBUG
	scope.setup(2, context->audioSampleRate);
	#endif
	
	return true;
}

void taskScheduler() {

	// call Bela_scheduleAuxiliaryTask() at
	// the interval specified by READ_HZ
	static unsigned sampleCount = 0;
	
	if(++sampleCount >= readSamps) {
		Bela_scheduleAuxiliaryTask(readTask);
		sampleCount = 0;
	}
}

bool blinkLed(bool reset = false) {
	
	static unsigned sampsCount = 0;
	static bool ledState = true;
	
	// we call this function with an argument of true to reset the rhythm
	if(reset) {
		sampsCount = 0;
		ledState = true;
		return ledState;
	}
	
	// otherwise increment and when we reach the specified interval 
	if(++sampsCount >= ledBlinkSamps) {
		
		// switch the LED on / off
		ledState = !ledState;
		
		// reset timer
		sampsCount = 0;
	}
	return ledState;
}

void bilinInterp(const float xValue, const float yValue, const int param, float output[weights::NUM_INPUTS]) {
	
	using namespace weights;
	
	for(int i = 0; i < NUM_INPUTS; ++i) {
		
		// perform horizontal interpolation between bottom corner points
		float xY1 = (1.f - xValue) * paramWeights[0][param][i] + xValue * paramWeights[1][param][i];
		
		// perform horizontal interpolation between top corner points
		float xY2 = (1.f - xValue) * paramWeights[2][param][i] + xValue * paramWeights[3][param][i];
		
		// Perform vertical interpolation between the two horizontally-interpolated values
		output[i] = (1.f - yValue) * xY1 + yValue * xY2;
	}
}

void render(BelaContext *context, void *userData) {
	
	static float mastOutLevel = 0.f;
	static unsigned recSampsCount = 0;
	static bool hasRecorded = false;
	static RecordState currentState = OFF;
	static RecordState prevState = OFF;
	
	for(int n = 0; n < context->audioFrames; ++n) {
		
		// record button press detected...
		if(recButton.process(!digitalRead(context, n, REC_BUTTON_PIN))) {
			
			// reset sample counter & LED blink rhythm
			recSampsCount = 0;
			blinkLed(true);
			
			// start recording bar readings & switch to record mode
			for(auto& bar : bars) bar.startRec();
			if(currentState != RECORD) {
				hasRecorded = true;
				currentState = RECORD;
			}
		}
		
		// loop button press detected...
		if(loopButton.process(!digitalRead(context, n, LOOP_BUTTON_PIN))) {
			
			if(currentState == RECORD || (currentState == OFF && hasRecorded)) {
				
				for(auto& bar : bars) bar.startLoop();
				currentState = LOOP;
			}
			
			// if in loop state, turn it off
			else if(currentState == LOOP) {
				for(auto& bar : bars) bar.stopLoop();
				currentState = OFF;
			}
		}
		
		// when in record state...
		if(currentState == RECORD) {
			
			// blink LED
			digitalWriteOnce(context, n, REC_LED_PIN, blinkLed());
			
			// once maximum record time is reached, switch to loop state
			if(++recSampsCount >= maxRecSamps) {
				currentState = LOOP;
				for(auto& bar : bars) bar.startLoop();
			}
		}
		
		// when the state has changed...
		if(currentState != prevState) {
			
			switch(currentState) {
				
				case OFF:
				
					if(prevState == LOOP) digitalWrite(context, n, LOOP_LED_PIN, false);
					break;
					
				case RECORD:
				
					if(prevState == LOOP) digitalWrite(context, n, LOOP_LED_PIN, false);
					break;
					
				case LOOP:
				
					if(prevState == RECORD) digitalWrite(context, n, REC_LED_PIN, false);
					digitalWrite(context, n, LOOP_LED_PIN, true);
					break;
			}
			prevState = currentState;
		}
		
		// update sensor values
		taskScheduler();
		
		float barValues[NUM_BARS];
		for(int i = 0; i < NUM_BARS; ++i) {
			
			// read switches & update bar if bars should hold their value
			float latchValue = latchDebouncers[i].process(!digitalRead(context, n, LATCH_PINS[i]));
			bars[i].setLatch(latchValue);

			barValues[i] = bars[i].getInterpValue();
		}
		
		float weightedValues[weights::NUM_PARAMS];
		
		int a = weights::NUM_PARAMS / 4;
		
		// calculate the weighted parameter values
		for(int param = 0; param < a; ++param) {
			
			for(int p = 0; p < 4; ++p) {
				
				float interpWeights[weights::NUM_INPUTS];
				
				// calculate interpolated parameter weights
				bilinInterp(square.getXValue(), square.getYValue(), (param + p * 4), interpWeights);
				
				// calculate dot product of inputs & corresponding weights
				float value = dot4_c(barValues, interpWeights);
				
				// limit value betwee 0 & 1
				weightedValues[param + p * 4] = utils::clamp(value);
			}
		}
		
		// float pulseHz = 2.f + 28.f * weightedValues[0];
		
		float pulseHz = PULSE_HZ_BASE * powf_neon(2.f, weightedValues[0] * PULSE_OCTAVES);
		pulse.setHz(pulseHz);
		
		for(int i = 0; i < NUM_TRAINS; ++i) {
			
			int baseParam = 1 + i * 4;
			
			trains[i].setGain(1.f - weightedValues[baseParam]);
			
			float windowRatio = WIN_RATIO_MIN + (WIN_RATIO_MAX - WIN_RATIO_MIN) * weightedValues[baseParam+1];
			trains[i].setOscHz(pulseHz * windowRatio);
			
			float oscHz = OSC_HZ_BASE * powf_neon(2.f, weightedValues[baseParam+2] * OSC_OCTAVES);
			trains[i].setOscHz(oscHz);
			
			trains[i].setStochProb(1.f - weightedValues[baseParam+3]);
		}
		
		// trigger pulsarets
		if(pulse.fallingEdge()) for(auto& train : trains) train.trigger();
		
		// sum pulsar train outputs
		float out[2] = {0.f, 0.f};
		for(auto& train : trains) train.process(out);
		
		// analog rate
		if(n % analogPerAudioSamps == 0) {
			
			// read and map master volume pot
			mastOutLevel = analogRead(context, n / analogPerAudioSamps, MAST_OUT_PIN);
			mastOutLevel = 1.f - map(mastOutLevel, 0.f, 3.3f / 4.096f, 0.f, 1.f);
			mastOutLevel = utils::clamp(mastOutLevel);
			mastOutLevel = mastOutputSmooth.process(mastOutLevel);
			
			// light LED according to bar value
			for(int i = 0; i < NUM_BARS; ++i) {
				float gammaValue = powf_neon(bars[i].getValue(), INVERSE_LED_GAMMA);
				analogWriteOnce(context, n / analogPerAudioSamps, BAR_LED_PINS[i], gammaValue);
			}
		}
		
		for(int chan = 0; chan < AUDIO_OUTS; ++chan) {
			
			// when active, apply anti-aliasing
			if(antiAliasing) out[chan] = antiAlias[chan].process(out[chan]);
			
			// scale output by master out level
			out[chan] *= mastOutLevel;
			
			// send audio to outputs
			audioWrite(context, n, chan, out[chan]);
		}
		
		#ifdef DEBUG
		scope.log(out);
		#endif
	}
}
void cleanup(BelaContext *context, void *userData) {}