#ifndef TOUCH_SQUARE_H
#define TOUCH_SQUARE_H
#include "TouchSensor.h"

class TouchSquare : public TouchSensor {
	
	public:
	
		static constexpr int NUM_DIMS = 2;
	
		TouchSquare() {
			
			// set sensor type to square
			device_ = Trill::SQUARE;
			
			// set number of sensor dimensions
			numDims_ = NUM_DIMS;
			
			// set default values
			for(int i = 0; i < numDims_; ++i) {
				values_[i] = 0.f;
				prevValues_[i] = 0.f;
			}
		}
		void read() override;
		float getXValue();
		float getYValue();
		
	private:
	
		float values_[NUM_DIMS];
		float prevValues_[NUM_DIMS];
};
#endif // TOUCH_SQUARE_H