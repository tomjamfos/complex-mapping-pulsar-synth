#include "Wavetable.h"

float Wavetable::process() {
	
	// if table hasn't been setup, return 0
	if(!tableSize_ || !table_) return 0.f;
	
	// get interpolated value from table
	float out = lerp(phaseIndex_);
	
	// add increment to phase, wrapping when we reach end of table 
	phaseIndex_ += phaseIncrement_;
	while(phaseIndex_ >= (float)tableSize_) phaseIndex_ -= (float)tableSize_;
	
	return out; 
}