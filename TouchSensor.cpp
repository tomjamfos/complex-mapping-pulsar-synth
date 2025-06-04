#include "TouchSensor.h"

bool TouchSensor::setup(uint8_t address, 
						Direction direction,
						bool useInterp,
						unsigned samples,
						float noiseThreshold) {
	
	// setup the sensor, function returns false if unsuccessful 
	if(sensor_.setup(1, device_, address) != 0) return false;
	
	// setup noise threshold & sensor physical orientation
	sensor_.setNoiseThreshold(noiseThreshold);
	direction_ = direction;
	
	// when using interpolation, set it up
	interpActive_ = useInterp;
	if(interpActive_) {
		valueInterp_.resize(numDims_);
		for(auto& interp : valueInterp_) {
			interp.setTime(samples);
		}
	}
	return true;
}

void TouchSensor::read() {
	
	// read i2c & check for active touch
	sensor_.readI2C();
	activeTouch_ = (bool)sensor_.getNumTouches();
}

bool TouchSensor::touchChanged() {
	
	// when touch is made or released returns true, else false
	if(activeTouch_ != prevActiveTouch_) {
		prevActiveTouch_ = activeTouch_;
		return true;
	}
	return false;
}