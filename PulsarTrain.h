#ifndef PULSAR_TRAIN_H
#define PULSAR_TRAIN_H
#include <vector>
#include "Pulsaret.h"
#include "DCBlock.h"
using std::vector;

class PulsarTrain {
	
	public:
	
		static constexpr int NUM_OUTPUTS = 2;
		static constexpr int MAX_OVERLAP = 4;
		static constexpr float MIN_DB = -45.f;
		static float dBRange;
		
		enum chanMaskMode {
			
			TRIGGER,
			UNBURST,
			BURST_CYCLE
		};
		
		void setup(float sampleRate, 
					const vector<float>& oscTable, 
					const vector<float>& windowTable,
					const vector<vector<float>>& panTables,
					bool dcBlock = true);
		inline void setOscHz(float Hz) {oscHz_ = Hz;}
		inline void setWindowHz(float Hz) {windowHz_ = Hz;}
		inline void setWindowMs(float ms) {windowHz_ = 1000.f / ms;}
		void setBurstRatio(int on, int off, bool invert = false);
		void setStochProb(float value = 1.f);
		void setChanMaskMode(chanMaskMode mode = UNBURST);
		void setChanRatio(int on, int off, bool invert = false);
		void setChanWidth(float width);
		void setPanPos(float pan);
		void setPanVari(float value);
		void setGain(float value);
		void trigger();
		void process(float (&outputs)[NUM_OUTPUTS]);
		
	private:
	
		// pulsarets
		Pulsaret pulsarPool_[MAX_OVERLAP];
		float oscHz_;
		float windowHz_;
		float panPos_;
		float panVari_;
		float level_;

		// burst masking
		bool burstMaskActive_;
		bool burstInvert_;
		int burstOn_;
		int burstTotal_;
		int burstCounter_;
		bool burstMask();
		
		// stochastic masking
		bool stochMaskActive_;
		float stochProb_;
		bool stochMask();
		
		// channel masking
		bool chanMask_;
		bool chanInvert_;
		int chanOn_;
		int chanTotal_;
		int chanCounter_;
		float chanWidth_;
		chanMaskMode chanCountMode_;
		
		// psuedo-random number generation
		float inverseRandMax_;
		float randValue(float min = 0.f, float max = 1.f);
		
		// DC blocking
		DCBlock dcBlock_[NUM_OUTPUTS];
		bool dcBlockActive_;
};
#endif // PULSAR_TRAIN_H