#pragma once

#include <string>
#include <cmath>

#define STRINGER_MAX_WEIGHT (uint64_t)63
#define STRINGER_MAX_DISTANCE (uint64_t)(-1)
#define STRINGER_DISTANCE_CHARS(N) ((uint64_t)1 << (STRINGER_MAX_WEIGHT - N))
#define STRINGER_DEFAULT_MATCH (uint64_t)3

using namespace std;

/**
* A pack of utility functions for string handling.
*/
class Stringer
{
public:
	/**
	* Assigns a distance score based on character matching.
	* @param str1 First string to compare.
	* @param str2 Second string to compare.
	*/
	static const uint64_t fuzzyDistance(const char* str1, const char* str2);

	/**
	* Assigns a distance score based on character matching.
	* @param str1 First string to compare.
	* @param str2 Second string to compare.
	*/
	static const uint64_t fuzzyDistance(string str1, string str2);
	
	/**
	* Returns true if the two strings match all characters (of the shortest one).
	* @param str First string to compare.
	* @param pattern Second string to compare.
	*/
	static const bool startsWithPattern(const char* str, const char * pattern);

	/**
	* Returns true if the two strings match at least some amount of characters.
	* @param str1 First string to compare.
	* @param str2 Second string to compare.
	* @param matches The amount of chars to match.
	*/
	static const bool isCloseEnough(const char* str1, const char* str2, uint8_t matches = STRINGER_DEFAULT_MATCH);

	/**
	* Returns true if the two strings match at least some amount of characters.
	* @param str1 First string to compare.
	* @param str2 Second string to compare.
	* @param matches The amount of chars to match.
	*/
	static const bool isCloseEnough(const string str1, const string str2, uint8_t matches = STRINGER_DEFAULT_MATCH);

	static void replacePattern(string& source, string oldPattern, string newPattern);
};