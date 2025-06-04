#include "LinRamp.h"

void LinRamp::rampTo(float value) {
	
	rampTo(value, rampSamples_);
}

void LinRamp::rampTo(float value, unsigned samples) {
	
	sampleCounter_ = samples;
	
	// how much should the value change per sample?
	increment_ = (value - value_) / (float)samples;
}

float LinRamp::process() {
	
	// whilst counter > 0, add increment to value
	if(sampleCounter_) {
		value_ += increment_;
		sampleCounter_--;
	}
	return value_;
}