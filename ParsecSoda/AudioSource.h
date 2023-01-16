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
#include "MetadataCache.h"

#define AUDIOSRC_BUFFER_SIZE 7056
#define AUDIOSRC_FORCE_RELEASE -1
#define AUDIOSRC_PREVIEW_MIN_DB -60
#define AUDIOSRC_PREVIEW_MAX_AMP 32768
#define AUDIOSRC_PREVIEW_MIN_AMP 0.001f

/**
* Buffer Size explained.
* 44100 Hz / 100  == 441 == 3² * 7²
* 48000 Hz / 1000 == 48  == 2^4 * 3
* GCD(441, 48) == 2^4 * 3² * 7² == 7056
* This is probably the best common ground.
*/
#define AUDIOSRC_SAMPLES_PER_BUFFER 7056 / 4

using namespace std;

typedef struct AudioSourceDevice
{
	std::string name;
	std::string id;
	size_t index;
} AudioSourceDevice;

#define CLSID_MMDeviceEnumerator ((const CLSID) __uuidof(MMDeviceEnumerator))
#define IID_IMMDeviceEnumerator ((const IID) __uuidof(IMMDeviceEnumerator))
#define IID_IAudioClient ((const IID) __uuidof(IAudioClient))
#define IID_IAudioCaptureClient ((const IID) __uuidof(IAudioCaptureClient))

class AudioSource
{
public:
	bool setDevice(int index = 0);
	void fetchDevices();
	virtual void setFrequency(uint32_t frequency);
	const uint32_t getFrequency() const;

	bool isReady();
	const vector<AudioSourceDevice> getDevices();
	void captureAudio();
	const vector<int16_t> popBuffer();
	const int popPreviewDecibel();
	const float* getPlot();
	const void togglePlot(const bool enabled);

	float volume = 1.0f;
	bool isEnabled = true;

	AudioSourceDevice currentDevice;


protected:
	void setEDataFlow(EDataFlow eDataFlow);
	void setStreamFlags(DWORD streamFlags);

	void swapBuffers();

	EDataFlow m_eDataFlow = eRender;
	DWORD m_streamFlags = AUDCLNT_STREAMFLAGS_LOOPBACK;

	IAudioCaptureClient* m_pCaptureClient = nullptr;
	size_t m_channels = 2;


	vector<AudioSourceDevice> m_devices;
	vector<int16_t> m_buffers[2];
	int m_activeBuffer = 0;
	bool m_isReady = false;
	int m_previewIndex = 0;
	bool m_isPlotActive = false;
	float m_plotBuffer[AUDIOSRC_SAMPLES_PER_BUFFER];

	size_t m_maxBufferSize = 0;


	uint32_t m_frequency = 44100;

	mutex m_mutex;
};