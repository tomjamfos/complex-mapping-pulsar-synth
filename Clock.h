#ifndef CLOCK_H
#define CLOCK_H
#include <cstdint>

class Clock {
	
	public:
	
		Clock() {
			
			rampScaler_ = 0.f;
			phase_ = 0;
			phaseIncrement_ = 0;
		}
		inline void setSampleRate(float sampleRate) {rampScaler_ = UINT32_MAX / sampleRate;}
		inline void setHz(float Hz) {phaseIncrement_ = Hz * rampScaler_;}
		inline bool fallingEdge() {
			
			// return true when phase wraps, this function also increments
			// phase accumulator so must be called from the render function
			phase_ += phaseIncrement_;
			return phase_ < phaseIncrement_;
		}
		
	private:
		
		float rampScaler_;
		uint32_t phase_;
		uint32_t phaseIncrement_;
		
};
#endif // CLOCK_H