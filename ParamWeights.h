#ifndef PARAM_WEIGHTS_H
#define PARAM_WEIGHTS_H

namespace weights {
	
	constexpr int NUM_DIMS = 4;
	constexpr int NUM_INPUTS = 4;
	
	// must be a multiple of 4!
	constexpr int NUM_PARAMS = 16;
	
	constexpr float paramWeights[NUM_DIMS][NUM_PARAMS][NUM_INPUTS] = {
		
																		// x1y1 (bottom left)
																		{
																			// pulsar rate
																			{.333f, 0.f, 0.f, 0.f},
																			
																			// train 1 level (inverse)
																			{0.f, 0.f, .25f, 0.f},
																			
																			// train 1 window ratio
																			{0.f, 1.f, 0.f, 0.f},
																			
																			// train 1 osc Hz
																			{0.f, 0.f, 1.f, 0.f},
																			
																			// train 1 stoch masking
																			{0.f, 0.f, 0.f, 0.f},
																			
																			// train 2 level (inverse)
																			{0.f, -.2f, .3f, .55f},
																			
																			// train 2 window ratio
																			{0.f, 0.f, 1.f, 0.f},
																			
																			// train 2 osc Hz
																			{.45f, 0.f, .1f, .2f},
																			
																			// train 2 stoch masking
																			{0.f, 0.f, .8f, 0.f},
																			
																			// train 3 level (inverse)
																			{0.f, 0.f, .35f, 0.f},
																			
																			// train 3 window ratio
																			{0.f, 0.f, 0.f, 1.f},
																			
																			// train 3 osc Hz
																			{0.f, 0.f, .65f, 0.f},
																			
																			// train 2 stoch masking
																			{.5f, 0.f, 0.f, -.1f},
																			
																			// empty
																			{0.f, 0.f, 0.f, 0.f},
																			
																			// empty
																			{0.f, 0.f, 0.f, 0.f},
																			
																			// empty
																			{0.f, 0.f, 0.f, 0.f}
																		},
																		
																		// x2y1 (bottom right)
																		{
																			// pulsar rate
																			{0.f, .5f, .5f, 0.f},
																			
																			// train 1 level (inverse)
																			{0.f, 0.f, .25f, 0.f},
																			
																			// train 1 window ratio
																			{0.f, 0.f, 1.f, 0.f},
																			
																			// train 1 osc Hz
																			{0.f, .4f, -.2f, 1.f},
																			
																			// train 1 stoch masking
																			{0.f, 0.f, .3f, 0.f},
																			
																			// train 2 level (inverse)
																			{.125f, 0.f, 0.f, .28f},
																			
																			// train 2 window ratio
																			{.333f, 0.f, .1f, 0.f},
																			
																			// train 2 osc Hz
																			{0.f, .2f, 0.f, 0.f},
																			
																			// train 2 stoch masking
																			{.125f, 0.f, 0.f, .333f},
																			
																			// train 3 level (inverse)
																			{0.f, .25f, 0.f, 0.f},
																			
																			// train 3 window ratio
																			{-.1f, .65f, 0.f, .2f},
																			
																			// train 3 osc Hz
																			{.1f, 0.f, .65f, 0.f},
																			
																			// train 3 stoch masking
																			{0.f, 0.f, .333f, 0.f},
																			
																			// empty
																			{0.f, 0.f, 0.f, 0.f},
																			
																			// empty
																			{0.f, 0.f, 0.f, 0.f},
																			
																			// empty
																			{0.f, 0.f, 0.f, 0.f}
																		},
																		
																		// x1y2 (top left)
																		{
																			// pulsar rate
																			{.135f, -.3f, 1.f, .2f},
																			
																			// train 1 level (inverse)
																			{0.f, .12f, -.2f, .5f},
																			
																			// train 1 window ratio
																			{0.f, .28f, -.2f, .125f},
																			
																			// train 1 osc Hz
																			{.23f, .1f, -.1f, .4f},
																			
																			// train 1 stoch masking
																			{0.f, .4f, 0.f, 0.f},
																			
																			// train 2 level (inverse)
																			{.1f, 0.f, .333f, 0.f},
																			
																			// train 2 window ratio
																			{1.f, 0.f, 0.f, 0.f},
																			
																			// train 2 osc Hz
																			{0.f, .65f, 0.f, .1f},
																			
																			// train 2 stoch masking
																			{0.f, 0.f, 0.f, 0.f},
																			
																			// train 3 level (inverse)
																			{0.f, 0.f, 0.f, .25f},
																			
																			// train 3 window ratio
																			{-.25f, 0.f, .3f, .1f},
																			
																			// train 3 osc Hz
																			{.1f, .6f, .445f, -.1f},
																			
																			// train 3 stoch masking
																			{0.f, 0.f, .2f, 0.f},
																			
																			// empty
																			{0.f, 0.f, 0.f, 0.f},
																			
																			// empty
																			{0.f, 0.f, 0.f, 0.f},
																			
																			// empty
																			{0.f, 0.f, 0.f, 0.f}
																		},
																		
																		// x2y2 (top right)
																		{
																			// pulsar rate
																			{.333f, -.25f, .35f, .25f},
																			
																			// train 1 level (inverse)
																			{0.f, .1f, .333f, 0.f},
																			
																			// train 1 window ratio
																			{-.8f, 0.f, .158f, .32f},
																			
																			// train 1 osc Hz
																			{-.2f, .45f, .24f, .1f},
																			
																			// train 1 stoch masking
																			{.5f, 0.f, .333f, -.1f},
																			
																			// train 2 level (inverse)
																			{.125f, 0.f, .5f, -.25f},
																			
																			// train 2 window ratio
																			{.333f, .14f, .9f, -.2f},
																			
																			// train 2 osc Hz
																			{0.f, .2f, -.1f, .5f},
																			
																			// train 2 stoch masking
																			{.1245f, 0.f, .58f, 0.f},
																			
																			// train 3 level (inverse)
																			{0.f, -.1f, .5f, .2f},
																			
																			// train 3 window ratio
																			{.6f, -.4f, .125f, .1f},
																			
																			// train 3 osc Hz
																			{.5f, 0.f, -.2f, .8f},
																			
																			// train 3 stoch masking
																			{.65f, .125f, -.25f, -.105f},
																			
																			// empty
																			{0.f, 0.f, 0.f, 0.f},
																			
																			// empty
																			{0.f, 0.f, 0.f, 0.f},
																			
																			// empty
																			{0.f, 0.f, 0.f, 0.f}
																		}
																	};
}
#endif // PARAM_WEIGHTS_H