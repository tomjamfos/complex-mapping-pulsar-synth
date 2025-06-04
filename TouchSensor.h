#ifndef TOUCH_SENSOR_H
#define TOUCH_SENSOR_H
#include <libraries/Trill/Trill.h>
#include "LinRamp.h"
#include <vector>
using std::vector;

class TouchSensor {
	
	public:
	
		// which way is the cable 
		// facing when viewed from above?
		enum Direction {
			UP,
			DOWN,
			LEFT,
			RIGHT
		};
	
		TouchSensor() {
			
			// set default device type
			device_ = Trill::NONE;
			
			// set default direction
			direction_ = LEFT;
			
			activeTouch_ = false;
			prevActiveTouch_ = false;
			
			// set default sensor dimensions
			numDims_ = 0;
			
			// interpolation off as default
			interpActive_ = false;
		}
		bool setup(uint8_t address, 
					Direction direction,
					bool useInterp,
					unsigned samples,
					float noiseThreshold = .025f);
		virtual void read();
		inline bool activeTouch() const {return activeTouch_;}
		bool touchChanged();
		
	protected:
	
		Trill sensor_;
		Trill::Device device_;
		Direction direction_;
		
		bool activeTouch_;
		bool prevActiveTouch_;
		
		int numDims_;
		vector<LinRamp> valueInterp_;
		bool interpActive_;
	
};
#endif // TOUCH_SENSOR_H