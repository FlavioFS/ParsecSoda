#pragma once
#include "AudioSource.h"


class AudioOut : public AudioSource
{
public:
	AudioOut();

	void setFrequency(Frequency frequency) override;
};