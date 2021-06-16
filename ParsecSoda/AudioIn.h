#pragma once

#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <mmsystem.h>
#include <string>
#include <vector>
#include <iostream>
#include <functiondiscoverykeys.h>
#include <initguid.h>
#include "Stringer.h"
#include "AudioTools.h"

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
	const int popPreviewDecibel();
	const std::vector<AudioInDevice> listInputDevices() const;
	AudioInDevice selectInputDevice(const char * name);

	float volume = 1.0f;
	bool isEnabled = true;

private:
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDevice *pDevice = NULL;
	IAudioClient *pAudioClient = NULL;
	IAudioCaptureClient *pCaptureClient = NULL;

	bool _isReady = false;
	int _activeBuffer = 0;
	int _previewIndex = 0;
};

