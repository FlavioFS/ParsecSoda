#pragma once
#include "AudioSource.h"

class AudioIn : public AudioSource
{
public:
	AudioIn();

	void setFrequency(Frequency frequency = Frequency::F44100) override;
};