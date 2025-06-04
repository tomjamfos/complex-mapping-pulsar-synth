#ifndef DEBOUNCER_H
#define DEBOUNCER_H

class Debouncer {
	
	public:
	
		Debouncer() {
			
			debounceSamples_ = 0;
			sampleCounter_ = 0;
			
			state_ = OFF;
		}
		void setup(float sampleRate, float debounceMs) {
			
			// convert from ms to samples
			debounceSamples_ = (int)(sampleRate * .001f * debounceMs);
		}
		bool process(bool value);
			
	private:
	
		unsigned debounceSamples_;
		unsigned sampleCounter_;
		enum {
			ON,
			JUST_ON,
			OFF,
			JUST_OFF
		} state_;
	
};
#endif // DEBOUNCER_H