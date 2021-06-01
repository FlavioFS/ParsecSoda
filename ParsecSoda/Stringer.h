#pragma once

#include <string>
#include <cmath>

#define STRINGER_MAX_WEIGHT 63
#define STRINGER_MAX_DISTANCE (uint64_t)(-1)
#define STRINGER_DISTANCE_CHARS(N) (1 << (STRINGER_MAX_WEIGHT - N))

class Stringer
{
public:
	static const uint64_t fuzzyDistance(const char* str1, const char* str2);
	static const uint64_t fuzzyDistance(std::string str1, std::string str2);
	static const bool startsWithPattern(const char* str, const char * pattern);
};