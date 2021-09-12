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

string Stringer::toLower(const string str)
{
	string result = str;
	for (size_t i = 0; i < result.size(); ++i)
	{
		result[i] = std::tolower(result[i]);
	}
	return result;
}

int Stringer::compareNoCase(const string a, const string b)
{
	const string aLower = Stringer::toLower(a);
	const string bLower = Stringer::toLower(b);
	return aLower.compare(bLower);
}

void Stringer::replacePattern(string& source, string oldPattern, string newPattern)
{
	size_t index = 0;
	while (true)
	{
		index = source.find(oldPattern, index);
		if (index == std::string::npos) break;

		source.replace(index, oldPattern.size(), newPattern);
		index++;
	}
}

void Stringer::replacePatternOnce(string& source, string oldPattern, string newPattern)
{
	size_t index = 0;
	vector<size_t> positions;

	while (true)
	{
		index = source.find(oldPattern, index);
		if (index == std::string::npos) break;

		positions.push_back(index);
		index++;
	}

	size_t offset = 0;
	int offsetSize = newPattern.size() - oldPattern.size();
	for (int i = 0; i < positions.size(); i++)
	{
		source.replace(positions[offset] + (int)(offset * offsetSize), oldPattern.size(), newPattern);
		offset++;
	}
}