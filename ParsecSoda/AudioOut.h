#pragma once

#include <mmdeviceapi.h>
#include <mmsystem.h>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <functiondiscoverykeys.h>
#include <Audioclient.h>
#include <initguid.h>
#include "Stringer.h"
#include "AudioTools.h"
#include "Frequency.h"
#include "MetadataCache.h"

#define AUDIO_OUT_FORCE_RELEASE -1
#define AUDIOOUT_PREVIEW_MIN_DB -60
#define AUDIOOUT_PREVIEW_MAX_AMP 32768
#define AUDIOOUT_PREVIEW_MIN_AMP 0.001f

using namespace std;

typedef struct AudioOutDevice
{
	std::string name;
	std::string id;
	size_t index;
} AudioOutDevice;

class AudioOut
{
public:
	bool setOutputDevice(int index = 0);
	void fetchDevices();
	void setFrequency(Frequency frequency = Frequency::F44100);
	Frequency getFrequency();
	//AudioOutputDevice selectOutputDevice(const char * name);
	//const std::vector<AudioOutputDevice> listOutputDevices() const;

	bool isReady();
	const vector<AudioOutDevice> getDevices();
	void captureAudio();
	const vector<int16_t> popBuffer();
	const int popPreviewDecibel();
	const uint32_t getFrequencyHz() const;

	float volume = 1.0f;
	bool isEnabled = true;

	AudioOutDevice currentDevice;

private:
	bool releaseDevices(HRESULT hr = AUDIO_OUT_FORCE_RELEASE);
	bool releaseDeviceCollection(HRESULT hr = AUDIO_OUT_FORCE_RELEASE);
	void swapBuffers();
	
	vector<AudioOutDevice> _devices;
	vector<int16_t> _outBuffers[2];
	int _activeBuffer = 0;
	int _isReady = false;
	int _previewIndex = 0;

	size_t maxOutBufferSize = 0;

	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
	const IID IID_IAudioClient = __uuidof(IAudioClient);
	const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

	uint32_t _frequency = 44100;

	mutex _mutex;
};

