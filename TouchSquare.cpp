#include "TouchSquare.h"
#include "Utils.h"

void TouchSquare::read() {
	
	TouchSensor::read();
	
	if(activeTouch_) {
		
		// get touch locations
		float x = sensor_.touchHorizontalLocation(0);
		float y = sensor_.touchLocation(0);
		
		// adjust value to account for physical orientation of sensor
		switch(direction_) {
			
			case UP:
			
				values_[0] = 1.f - y;
				values_[1] = x;
				break;
				
			case DOWN:
			
				values_[0] = y;
				values_[1] = 1.f - x;
				break;
				
			case LEFT:
			
				values_[0] = 1.f - x;
				values_[1] = 1.f - y;
				break;
				
			case RIGHT:
			
				values_[0] = x;
				values_[1] = y;
				break;
			
		}
		
		for(int i = 0; i < NUM_DIMS; ++i) {
			
			// limit value between 0 & 1
			values_[i] = utils::clamp(values_[i]);
			
			// if interpolation is active and the 
			// value has changed, ramp to new value
			if(interpActive_) {
				if(values_[i] != prevValues_[i]) {
					valueInterp_[i].rampTo(values_[i]);
					prevValues_[i] = values_[i];
				}
			}
		}	
	}
}

float TouchSquare::getXValue() {
	
	float out;
	
	// if interpolation is active use that, otherwise use raw value
	if(interpActive_) out = valueInterp_[0].process();
	else out = values_[0];
	
	return out;
}

float TouchSquare::getYValue() {
	
	float out;
	
	// if interpolation is active use that, otherwise use raw value
	if(interpActive_) out = valueInterp_[1].process();
	else out = values_[1];
	
	return out;
}