#include "Debouncer.h"

bool Debouncer::process(bool value) {
	
	switch(state_) {
		
		case ON:
		
			// if off value recieved, switch state & reset counter
			if(!value) {
				state_ = JUST_OFF;
				sampleCounter_ = 0;
			}
			break;
			
		case JUST_ON:
			
			// once debounce time has elapsed, switch state to on
			if(++sampleCounter_ >= debounceSamples_) {
				state_ = ON;
			}
			break;
			
		case OFF:
		
			// if on value recieved, switch state & reset counter
			if(value) {
				state_ = JUST_ON;
				sampleCounter_ = 0;
			}
			break;
			
		case JUST_OFF:
		
			// once debounce time has elapsed, switch state to off
			if(++sampleCounter_ >= debounceSamples_) {
				state_ = OFF;
			}
			break;
	}
	
	if(state_ == ON || state_ == JUST_ON) return true;
	else return false;
}