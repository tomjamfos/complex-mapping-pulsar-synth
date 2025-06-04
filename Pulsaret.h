#ifndef PULSARET_H
#define PULSARET_H
#include <vector>
#include "Wavetable.h"
#include "Window.h"
#include "Pan.h"
using std::vector;

class Pulsaret {
	
	public:
	
		static constexpr int NUM_OUTPUTS = 2;
	
		void setup(float sampleRate, 
					const vector<float>& oscTable, 
					const vector<float>& windowTable, 
					const vector<vector<float>>& panTables);
		void trigger(float oscHz, float windowHz, float panPos = .5f, float level = 1.f);
		void process(float (&outputs)[NUM_OUTPUTS]);
		inline bool isActive() const {return window_.isActive();}
		
	private:
	
		Wavetable osc_;
		Window window_;
		Pan panning_[NUM_OUTPUTS];
	
};
#endif // PULSARET_H