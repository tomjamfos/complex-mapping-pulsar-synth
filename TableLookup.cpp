#include "TableLookup.h"

float TableLookup::lerp(float phase) const {
	
	const vector<float>& table = *table_;
	
	// calculate which indicies the phase index falls between,
	// wrapping when it falls past the final value of table
	int indexBelow = (int)phase;
	int indexAbove = indexBelow + 1;
	if(indexAbove >= tableSize_) indexAbove = 0;
	
	// how far is the phase index from above & below indicies?
	float fractAbove = phase - (float)indexBelow;
	float fractBelow = 1.f - fractAbove;
	
	// perform linear interpolation
	return fractBelow * table[indexBelow] + fractAbove * table[indexAbove];
}