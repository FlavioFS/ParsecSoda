#include "AudioMix.h"

const std::vector<int16_t>& AudioMix::mix(const std::vector<int16_t>& buffer1, const std::vector<int16_t>& buffer2)
{
	static std::vector<int16_t> mixBuffer;
	size_t shorter = std::min(buffer1.size(), buffer2.size());
	mixBuffer.resize(shorter);

	for (size_t i = 0; i < shorter; i++)
	{
		mixBuffer[i] = buffer1[i] + buffer2[i];
	}

	return mixBuffer;
}