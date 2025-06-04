/*
	Utility functions used throughout,
	currently consists only of a clamp function
*/

#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <cmath>

namespace utils {
	
	template <typename T>
	constexpr T clamp(T value, T min = T{0}, T max = T{1}) {
		
		return (value < min) ? min : ((value > max) ? max : value); 
	}
}
#endif // UTILS_H