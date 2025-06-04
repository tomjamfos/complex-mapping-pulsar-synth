#include "Butterworth4.h"
#include "Utils.h"

void Butterworth4::setup(float sampleRate, float nyqFract) {
	
	nyqFract = utils::clamp(nyqFract);
	
	double cutoff = sampleRate * .5 * nyqFract;
	
	BiquadCoeff::Settings settings;
	settings.fs = sampleRate;
	settings.type = BiquadCoeff::lowpass;
	settings.cutoff = cutoff;
	settings.peakGainDb = 0;
	
	settings.q = .5412;
	stage1.setup(settings);
	
	settings.q = 1.3065;
	stage2.setup(settings);
}

float Butterworth4::process(float value) {
	
	float stage1Out = stage1.process(value);
	return stage2.process(stage1Out);
}