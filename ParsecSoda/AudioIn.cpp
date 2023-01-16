#include "AudioIn.h"

AudioIn::AudioIn()
{
	this->setEDataFlow(eCapture);
	this->setStreamFlags(0);
}

void AudioIn::setFrequency(Frequency frequency)
{
	AudioSource::setFrequency(frequency);
	MetadataCache::preferences.micFrequency = m_frequency;
	MetadataCache::savePreferences();
}