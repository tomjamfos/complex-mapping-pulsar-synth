#ifndef TOUCH_BAR_H
#define TOUCH_BAR_H
#include <vector>
#include "TouchSensor.h"
using std::vector;

class TouchBar : public TouchSensor {
	
	public:
	
		TouchBar();
		void setRecTime(float seconds, float readRate);
		inline void startRec() {
			
			// only when touch is made with the sensor
			if(activeTouch_) {
				
				// reset record playback head
				recordHead_ = 0;
				
				// make sure looping is off
				loopActive_ = false;
				
				// activate recording
				recordActive_ = true;
				
				// add flag so that looping can be activated
				hasRecorded_ = true;
			}
			
		}
		inline void startLoop() {
			
			// reset loop playback head
			playHead_ = 0;
			
			// make sure recording is off
			recordActive_ = false;
			
			// activate looping
			loopActive_ = true;
		}
		inline void stopLoop() {loopActive_ = false;}
		inline void setLatch(bool active) {latch_ = active;}
		void read() override;
		inline float getValue() {return value_;}
		float getInterpValue();
			
	private:
	
		bool latch_;
		float value_;
		float prevValue_;
		
		vector<float> recordBuffer_;
		bool recordActive_;
		bool hasRecorded_;
		unsigned recordHead_;
		
		bool loopActive_;
		unsigned playHead_;
	
};
#endif // TOUCH_BAR_H