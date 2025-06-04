/*
	A header for generating wavetables, should  
	be called from setup function of render.cpp,  
	keeping the resulting vector in global scope
*/
#ifndef WAVETABLES_H
#define WAVETABLES_H
#include <cmath>
#include <vector>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <utility>
#include <libraries/AudioFile/AudioFile.h>
#include "Utils.h"
using std::vector;
using std::pair;

constexpr float HALF_PI = (float)M_PI_2;
constexpr float PI = (float)M_PI;
constexpr float TWO_PI = 2.f * (float)M_PI;
constexpr size_t DEF_SIZE = 512;
constexpr size_t MIN_SIZE = 32;
constexpr size_t MAX_SIZE = 4096;
constexpr size_t DEF_HARMONICS = 10;
constexpr float MAX_CURVE = 12.f;

namespace {
	
	using std::function;
	
	inline void normaliseTable(vector<float>& table) {
		
		// find maximum absolute value in table
		float absMaxValue = 0.f;
		for(auto& value : table) {
			float absValue = fabsf(value);
			if(absValue > absMaxValue) absMaxValue = absValue;
		}
		
		// providing it is above 0, scale every
		// value in table by it's reciprocal
		if(absMaxValue > 0.f) {
			float normFactor = 1.f / absMaxValue;
			for(auto& value : table) {
				value *= normFactor;
			}
		}
	}
	
	template<bool IncludeEndpoint = false>
	inline vector<float> generateTable(size_t size, function<float(float)> generator) {
		
		// limit table size between min & max
		size = utils::clamp(size, MIN_SIZE, MAX_SIZE);
		
		vector<float> table(size);
		
		// is the table a window function or oscillator
		const float tableSize = IncludeEndpoint ? (float)(size - 1) : (float)size;
		
		// generate table
		for(size_t i = 0; i < size; ++i) {
			float normPhase = (float)i / tableSize;
			table[i] = generator(normPhase);
		}
		
		return table;
	}
	
	inline vector<float> genHarmonicTable(size_t harmonics, size_t size, function<float(float, size_t)> harmonicGenerator) {
		
		// limit table size between min & max
		size = utils::clamp(size, MIN_SIZE, MAX_SIZE);
		
		vector<float> table(size);
		
		// generate table
		for(size_t i = 0; i < size; ++i) {
			
			float normPhase = (float)i / (float)size;
			
			float value = 0.f;
			for(size_t h = 0; h < harmonics; ++h) {
				value += harmonicGenerator(normPhase, h);
			}
			
			table[i] = value;
		}
		
		// normalise wavetable to be between -1 & 1
		normaliseTable(table);
		
		return table;
	}
	
	inline vector<vector<float>> genPanTable(size_t size, function<pair<float, float>(float)> gen) {
		
		// limit table size between min & max
		size = utils::clamp(size, MIN_SIZE, MAX_SIZE);
		
		vector<vector<float>> table(2);
		for(auto& chan : table) chan.resize(size);
		
		// generate table
		for(size_t i = 0; i < size; ++i) {
			
			float normPhase = (float)i / (float)(size - 1);
			std::pair<float, float> values = gen(normPhase);
			table[0][i] = values.first;
			table[1][i] = values.second;
		}
		
		return table;
	}
}

namespace waveforms {
	
	inline vector<float> sine(size_t size = DEF_SIZE) {
		
		return generateTable(size, [](float phase) {
			return sinf(TWO_PI * phase);
		});
	}
	
	inline vector<float> triangle(size_t harmonics = DEF_HARMONICS, size_t size = DEF_SIZE) {
		
		return genHarmonicTable(harmonics, size, [](float phase, size_t harmonic) {
			float h = 2.f * (float)harmonic + 1.f;
			return powf(-1.f, (float)harmonic) * sinf(TWO_PI * h * phase) / (h * h);
		});
	}
	
	inline vector<float> sawtooth(size_t harmonics = DEF_HARMONICS, size_t size = DEF_SIZE) {
		
		return genHarmonicTable(harmonics, size, [](float phase, size_t harmonic) {
			float h = (float)(harmonic + 1);
			return sinf(TWO_PI * h * phase) / h;
		});
	}

	inline vector<float> square(size_t harmonics = DEF_HARMONICS, size_t size = DEF_SIZE) {
		
		return genHarmonicTable(harmonics, size, [](float phase, size_t harmonic) {
			float h = 2.f * (float)harmonic + 1.f;
			return sinf(2.f * PI * h * phase) / h;
		});
	}

	inline vector<float> noise(unsigned seed = 1, size_t size = DEF_SIZE) {
		
		// seed pseudo-random number generator 
		srand(seed);
		
		// fill wavetable with random values between -1 & 1
		return generateTable(size, [](float phase) {
			return (float)rand() / (float)RAND_MAX * 2.f - 1.f;
		});
	}
}

namespace windows {
	
	
	inline vector<float> sine(size_t size = DEF_SIZE) {
		
		return generateTable<true>(size, [](float phase) {
			return sinf(PI * phase);
		});
	}
	
	inline vector<float> linAttack(size_t size = DEF_SIZE) {
		
		return generateTable<true>(size, [](float phase) {
			return phase;
		});
	}
	
	inline vector<float> linDecay(size_t size = DEF_SIZE) {
		
		return generateTable<true>(size, [](float phase) {
			return 1.f - phase;
		});
	}

	inline vector<float> expDecay(float curve = .5f, size_t size = DEF_SIZE) {
		
		// limit curve between 0.001 & 1
		curve = utils::clamp(curve, .001f);
		
		const float curveAmount = curve * MAX_CURVE;
		
		return generateTable<true>(size, [curveAmount](float phase) {
			return (expf(-curveAmount * phase) - expf(-curveAmount)) / (1.f - expf(-curveAmount));
		});
	}

	inline vector<float> expAttack(float curve = .5f, size_t size = DEF_SIZE) {
		
		// limit curve between 0.001 & 1
		curve = utils::clamp(curve, .001f);
		
		const float curveAmount = curve * MAX_CURVE;
		
		return generateTable<true>(size, [curveAmount](float phase) {
			return (expf(curveAmount * phase) - 1.f) / (expf(curveAmount) - 1.f);
		});
	}
	
	inline vector<float> bartlett(size_t size = DEF_SIZE) {
		
		return generateTable<true>(size, [](float phase) {
			return 1.f - fabsf(2.f * phase - 1.f);
		});
	}
	
	inline vector<float> hann(size_t size = DEF_SIZE) {
		
		return generateTable<true>(size, [](float phase) {
			return .5f * (1.f - cosf(TWO_PI * phase));
		});
	}

	inline vector<float> hamming(size_t size = DEF_SIZE) {
		
		return generateTable<true>(size, [](float phase) {
			return 0.54f - 0.46f * cosf(TWO_PI * phase);
		});
	}

	inline vector<float> gaussian(float sigma = .125f, size_t size = DEF_SIZE) {
		
		// limit sigma between 0.001 & 1
		sigma = utils::clamp(sigma, .001f, 1.f);
		
		return generateTable<true>(size, [sigma](float phase) {
			float x = phase - .5f;
			return expf(-.5f * powf(x / sigma, 2.f));
		});
	}
	
	inline vector<float> tukey(float alpha = .5f, size_t size = DEF_SIZE) {
		
		// limit alpha between 0 & 1
		alpha = utils::clamp(alpha);
		
		const float halfAlpha = alpha * .5f;
		
		return generateTable<true>(size, [alpha, halfAlpha](float phase) {
			
			float value;
			
			if(phase < halfAlpha) value = .5f * (1.f - cosf(TWO_PI * phase / alpha));
			else if(phase > (1.f - halfAlpha)) value = .5f * (1.f - cosf(TWO_PI * (1.f - phase) / alpha));
			else value = 1.f;
			
			return value;
		});
	}
}

namespace pan {

	inline vector<vector<float>> constantPower(size_t size = DEF_SIZE) {
		
		return genPanTable(size, [](float phase) -> pair<float, float> {
			return {cosf(HALF_PI * phase), sinf(HALF_PI * phase)};
		});
	}

	inline vector<vector<float>> linear(size_t size = DEF_SIZE) {
		
		return genPanTable(size, [](float phase) -> pair<float, float> {
			return {1.f - phase, phase};
		});
	}
}

namespace sample {
	
	inline vector<float> loadSample(const std::string& filename, size_t offset = 0, size_t size = 4096) {
		
		// limit table size between min & max
		size = utils::clamp(size, MIN_SIZE, MAX_SIZE);
		
		// get number of frames in audio file
		const int numFrames = AudioFileUtilities::getNumFrames(filename);
		
		// return empty vector if audio file is shorter then specified size
		if(numFrames < (int)size) return vector<float>();
		
		if((offset + size) >= (size_t)numFrames) offset = (size_t)numFrames - size;
		
		vector<float> table(size);
		
		// get specified number of samples (starting from offset) 
		// from audio file, returning empty vector if error 
		if(AudioFileUtilities::getSamples(filename, table.data(), 0, offset, offset + size - 1) != 0) {
			return vector<float>();
		}
		
		// normalise wavetable to be between -1 & 1
		normaliseTable(table);
		
		return table;
	}
}
#endif // WAVETABLES_H