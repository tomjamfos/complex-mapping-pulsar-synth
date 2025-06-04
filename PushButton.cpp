#include "PushButton.h"

void PushButton::setup(float sampleRate, float debounceMs) {
	
	// setup debouncer
	debouncer_.setup(sampleRate, debounceMs);
}

bool PushButton::process(bool value) {
	
	value = debouncer_.process(value);
	
	if(value != prevValue_) {
		
		prevValue_ = value;
		
		if(value) return true;
	}
	return false;
}