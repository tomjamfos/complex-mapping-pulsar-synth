#ifndef LIN_RAMP_H
#define LIN_RAMP_H

class LinRamp {
	
	public:
	
		LinRamp() {
			
			// default values
			increment_ = 0.f;
			value_ = 0.f;
			sampleCounter_ = 0;
		}
		inline void setValue(float value) {
			
			// set a new value and restart counter
			value_ = value;
			sampleCounter_ = 0;
		}
		inline void setTime(unsigned samples) {rampSamples_ = samples;}
		void rampTo(float value);
		void rampTo(float value, unsigned samples);
		float process();
		inline bool isFinished() const {return !sampleCounter_;}
		
	private:
	
		unsigned rampSamples_;
		unsigned sampleCounter_;
		float increment_;
		float value_;
	
};
#endif // LIN_RAMP_H