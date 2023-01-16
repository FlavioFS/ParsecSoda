#pragma once
#include "AudioSource.h"


class AudioOut : public AudioSource
{
public:
	AudioOut();

	void setFrequency(uint32_t frequency = 48000) override;
};