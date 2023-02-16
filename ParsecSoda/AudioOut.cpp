#include "AudioOut.h"

AudioOut::AudioOut()
{
	this->setEDataFlow(eRender);
	this->setStreamFlags(AUDCLNT_STREAMFLAGS_LOOPBACK);
}