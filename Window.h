#ifndef WINDOW_H
#define WINDOW_H
#include "Wavetable.h"
#include "Utils.h"

class Window : public Wavetable {
	
	public:
	
		Window() {
			
			level_ = 1.f;
			
			// window inactive by default
			active_ = false;
		}
		void trigger(float level = 1.f);
		float process();
		inline bool isActive() const {return active_;}
		
	private:
	
		float level_;
		bool active_;
	
};

#endif // WINDOW_H