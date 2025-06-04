#ifndef WAVETABLE_H
#define WAVETABLE_H
#include "TableLookup.h"

class Wavetable : public TableLookup {
	
	public:
	
		Wavetable() {
			
			// sample rate & phase increment are 0 by default 
			inverseSR_ = 0.f;
			phaseIncrement_ = 0.f;
		}
		inline void setSampleRate(float sampleRate) {
			
			// calculate reciprocal of sample rate (to avoid division at audio rate)
			inverseSR_ = 1.f / sampleRate;
		}
		inline void setHz(float Hz) {
			
			// assuming we are changing the frequency at a rate lower than the sample rate,
			// it is more efficient to calculate the increment here than in process function
			phaseIncrement_ = (float)tableSize_ * Hz * inverseSR_;
		}
		void trigger() {
			
			// restart phase at beginning of table
			phaseIndex_ = 0.f;
		}
		float process();
		
	protected:
	
		float inverseSR_;
		float phaseIncrement_;
	
};
#endif // WAVETABLE_H