#ifndef PUSH_BUTTON_H
#define PUSH_BUTTON_H
#include "Debouncer.h"

class PushButton {
	
	public:
	
		PushButton() {prevValue_ = false;}
		void setup(float sampleRate, float debounceMs);
		bool process(bool value);
		
	private:
	
		Debouncer debouncer_;
		bool prevValue_;
	
};
#endif // PUSH_BUTTON_H