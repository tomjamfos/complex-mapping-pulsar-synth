#ifndef BUTTERWORTH_4_H
#define BUTTERWORTH_4_H
#include <libraries/Biquad/Biquad.h>

class Butterworth4 {
	
	public:
	
		void setup(float sampleRate, float nyqFract = .9f);
		float process(float value);
		
	private:
	
		Biquad stage1;
		Biquad stage2;
		
};
#endif // BUTTERWORTH_4_H