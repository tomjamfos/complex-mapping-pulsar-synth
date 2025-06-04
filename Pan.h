#ifndef PAN_H
#define PAN_H
#include "TableLookup.h"

class Pan : public TableLookup {
	
	public:
	
		Pan() {
			
			// centre pan position by default
			panPos_ = .5f;
		}
		inline void setPanPos(float pan) {panPos_ = pan;}
		float process();
		
	private:
	
		float panPos_;
	
};
#endif // PAN_H