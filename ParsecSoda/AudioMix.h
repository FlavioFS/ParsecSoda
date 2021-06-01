#pragma once

#include <vector>
#include <algorithm>

#define AUDIOMIX_DEFAULT_VOLUME 1.0

class AudioMix
{
public:
	AudioMix(float volume1 = AUDIOMIX_DEFAULT_VOLUME, float volume2 = AUDIOMIX_DEFAULT_VOLUME);
	void setVolumes(const float volume1 = AUDIOMIX_DEFAULT_VOLUME, const float volume2 = AUDIOMIX_DEFAULT_VOLUME);
	void setVolume1(const float volume = AUDIOMIX_DEFAULT_VOLUME);
	void setVolume2(const float volume = AUDIOMIX_DEFAULT_VOLUME);
	const std::vector<int16_t> mix(const std::vector<int16_t> *buffer1, const std::vector<int16_t> *buffer2) const;
private:
	float _volume1 = AUDIOMIX_DEFAULT_VOLUME, _volume2 = AUDIOMIX_DEFAULT_VOLUME;
};

