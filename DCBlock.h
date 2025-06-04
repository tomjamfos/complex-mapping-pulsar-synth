#ifndef DC_BLOCK_H
#define DC_BLOCK_H

// https://ccrma.stanford.edu/~jos/fp/DC_Blocker_Software_Implementations.html

class DCBlock {
	
	public:
	
		static constexpr float coeff = .995f;
	
		DCBlock() {
			
			xm1 = 0.f;
			ym1 = 0.f;
		}
	
		inline float process(float value) {
			
			float out = value - xm1 + coeff * ym1;
			xm1 = value;
			ym1 = out;
			return out;
		}
		
	private:
	
		float xm1;
		float ym1;
	
};
#endif // DC_BLOCK_H