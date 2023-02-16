#include "AudioIn.h"

AudioIn::AudioIn()
{
	this->setEDataFlow(eCapture);
	this->setStreamFlags(0);
}