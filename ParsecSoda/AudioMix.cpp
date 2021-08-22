#include "AudioMix.h"

const std::vector<int16_t>& AudioMix::mix(const std::vector<int16_t>& buffer1, const std::vector<int16_t>& buffer2)
{
	static std::vector<int16_t> mixBuffer;

	//size_t shorter = std::min(buffer1.size(), buffer2.size());
	size_t larger = std::max(buffer1.size(), buffer2.size());
	mixBuffer.resize(larger);

	for (size_t i = 0; i < mixBuffer.size(); i++)
	{
		mixBuffer[i] = 0;
		if (i < buffer1.size()) mixBuffer[i] += buffer1[i];
		if (i < buffer2.size()) mixBuffer[i] += buffer2[i];
	}


	return mixBuffer;
}