#pragma once
#include "AudioSource.h"

class AudioIn : public AudioSource
{
public:
	AudioIn();

	void setFrequency(uint32_t frequency = 48000) override;
};