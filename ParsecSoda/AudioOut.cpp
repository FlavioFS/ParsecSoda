#include "AudioOut.h"

AudioOut::AudioOut()
{
	this->setEDataFlow(eRender);
	this->setStreamFlags(AUDCLNT_STREAMFLAGS_LOOPBACK);
}

void AudioOut::setFrequency(uint32_t frequency)
{
	AudioSource::setFrequency(frequency);
	MetadataCache::preferences.speakersFrequency = m_frequency;
	MetadataCache::savePreferences();
}