#include "AudioMix.h"

AudioMix::AudioMix(const float volume1, const float volume2)
{
	setVolumes(volume1, volume2);
}

void AudioMix::setVolumes(const float volume1, const float volume2)
{
	setVolume1(volume1);
	setVolume2(volume2);
}

void AudioMix::setVolume1(const float volume)
{
	if (volume < 0.0)
	{
		return;
	}

	_volume1 = volume;
}

void AudioMix::setVolume2(const float volume)
{
	if (volume < 0.0)
	{
		return;
	}

	_volume2 = volume;
}

const std::vector<int16_t> AudioMix::mix(const std::vector<int16_t>* buffer1, const std::vector<int16_t>* buffer2) const
{
	size_t shorter = std::min(buffer1->size(), buffer2->size());
	std::vector<int16_t> mixBuffer;
	mixBuffer.resize(shorter);
	for (size_t i = 0; i < shorter; i++)
	{
		mixBuffer[i] = ((*buffer1)[i] * _volume1 + (*buffer2)[i] * _volume2);
	}

	return mixBuffer;
}
