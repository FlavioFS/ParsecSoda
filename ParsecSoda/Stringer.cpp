#include "Stringer.h"

const uint64_t Stringer::fuzzyDistance(const char * a, const char * b)
{
	return fuzzyDistance( std::string(a), std::string(b) );
}

const uint64_t Stringer::fuzzyDistance(std::string a, std::string b)
{
	uint64_t dab = 0;
	uint64_t weight = STRINGER_MAX_WEIGHT;
	size_t shortestLen =  a.length() <= b.length() ? a.length() : b.length();
	if (shortestLen > STRINGER_MAX_WEIGHT) shortestLen = STRINGER_MAX_WEIGHT;

	for (size_t i = 0; i < shortestLen; i++)
	{
		if (std::tolower(a[i]) != std::tolower(b[i]))
		{
			dab += ((uint64_t)1 << weight);
		}
		weight--;
	}
	
	return dab; // Dab! \o>
}

const bool Stringer::startsWithPattern(const char * str, const char * pattern)
{
	std::string a = str, b = pattern;

	if (a.length() < b.length()) { return false; }

	for (size_t i = 0; i < b.length(); i++)
	{
		if (std::tolower(a[i]) != std::tolower(b[i])) { return false; }
	}

	return true;
}

const bool Stringer::isCloseEnough(const char* str1, const char* str2, uint8_t matches)
{
	return Stringer::fuzzyDistance(str1, str2) <= STRINGER_DISTANCE_CHARS(matches);
}

const bool Stringer::isCloseEnough(const std::string str1, const std::string str2, uint8_t matches)
{
	return Stringer::isCloseEnough(str1.c_str(), str2.c_str(), matches);
}
