#include "Window.h"

void Window::trigger(float level) {
	
	// limit level between 0 & 1
	level_ = utils::clamp(level);
	
	// restart from beginning of table
	phaseIndex_ = 0.f;
	
	// activate window
	active_ = true;
}

float Window::process() {
	
	// if window is inactive or table hasn't been setup, return 0
	if(!active_ || !tableSize_ || !table_) return 0.f;
	
	// get interpolated value from table 
	// and scale according to window level
	float out = lerp(phaseIndex_) * level_;
	
	// add increment to phase, switching window
	// to inactive once table end reached
	phaseIndex_ += phaseIncrement_;
	
	// make window inactive once we reach the end
	if(phaseIndex_ >= (float)(tableSize_ - 1)) active_ = false;
	
	return out;
}