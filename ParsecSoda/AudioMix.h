#pragma once

#include <vector>
#include <algorithm>

class AudioMix
{
public:
	static const std::vector<int16_t>& mix(const std::vector<int16_t>& buffer1, const std::vector<int16_t>& buffer2);
};

