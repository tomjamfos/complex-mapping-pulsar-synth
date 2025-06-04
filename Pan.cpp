#include "Pan.h"

float Pan::process() {
	
	// if table hasn't been setup or is empty, return 0
	if(!tableSize_ || !table_) return 0.f;
	
	// which (fractional) index?
	phaseIndex_ = panPos_ * (float)(tableSize_ - 1);
	
	// get interpolated value from (fractional) index
	float out = lerp(phaseIndex_);
	
	return out;
}