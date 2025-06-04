#include "TouchBar.h"
#include <cmath>
#include "Utils.h"

TouchBar::TouchBar() {
	
	// set sensor device to bar
	device_ = Trill::BAR;
	
	// sensor is 1-dimensional
	numDims_ = 1;
	
	// latch off by default
	latch_ = false;
	
	// set initial value
	value_ = 0.f;
	prevValue_ = 0.f;
	
	// recording inactive by default
	recordActive_ = false;
	recordHead_ = 0;
	
	// looping inactive by default
	loopActive_ = false;
	playHead_ = 0;
}

void TouchBar::setRecTime(float seconds, float readRate) {
	
	recordBuffer_.resize(ceil(seconds * readRate));
}

void TouchBar::read() {
	
	TouchSensor::read();
	
	if(!loopActive_ || !hasRecorded_) {
		
		// when a touch is made...
		if(activeTouch_) {
		
			// get touch location
			value_ = sensor_.touchLocation(0);
		
			// limit between 0 & 1
			value_ = utils::clamp(value_);
		
			// adjust value to account for physical orientation of sensor
			if(direction_ == RIGHT || direction_ == DOWN) {
				value_ = 1.f - value_;
			}
	}
	else if(!latch_) value_ = 0.f;
		
		if(recordActive_) {
			
			recordBuffer_[recordHead_] = value_;
			if(++recordHead_ >= recordBuffer_.size()) startLoop();
		}
	}
	
	// when looping is active
	else if(loopActive_ && hasRecorded_) {
		
		// read value from recorded buffer
		value_ = recordBuffer_[playHead_];
		
		// increment & wrap play head
		if(++playHead_ >= recordHead_) playHead_ = 0;
	}
	
	// when sensor value changes, start a linear 
	// ramp to it (if interpolation is active)
	if(interpActive_ && value_ != prevValue_) {
		valueInterp_[0].rampTo(value_);
		prevValue_ = value_;
	}
}

float TouchBar::getInterpValue() {
	
	float out = 0.f;
	
	// if interpolation is active, get the results
	if(interpActive_) {
		out = valueInterp_[0].process();
	}
	
	return out;
}