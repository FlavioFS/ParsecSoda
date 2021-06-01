#pragma once

#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <mmsystem.h>
#include <string>
#include <vector>
#include <iostream>
#include "Stringer.h"
#include <functiondiscoverykeys.h>
#include <initguid.h>
#include "parsec-dso.h"

typedef struct AudioInDevice
{
	WAVEINCAPS wave;
	UINT id;
	bool isEmpty = true;
} AudioInDevice;

class AudioIn
{
public:
	bool init(AudioInDevice device);
	void captureAudio();
	const bool isReady() const;
	const std::vector<int16_t> popBuffer();
	const std::vector<AudioInDevice> listInputDevices() const;
	AudioInDevice selectInputDevice(const char * name);

private:
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDevice *pDevice = NULL;
	IAudioClient *pAudioClient = NULL;
	IAudioCaptureClient *pCaptureClient = NULL;

	bool _isReady = false;
	int _activeBuffer = 0;
};

