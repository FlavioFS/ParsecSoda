#include "AudioOut.h"


// ==================================================
//   Input Sources
// ==================================================
#define AUDIO_OUT_DEFAULT_FREQUENCY 44100
#define AUDIO_OUT_CHANNELS 2
#define AUDIO_OUT_SWAP_BUFFERS 2
#define AUDIO_OUT_BITS 32
#define AUDIO_OUT_BYTES_PER_SAMPLE (AUDIO_OUT_BITS/8)
#define AUDIO_OUT_SAMPLES_PER_BUFFER 4 * AUDIO_OUT_DEFAULT_FREQUENCY / 100


// ==================================================
//   Output Sources
// ==================================================
#define SAFE_RELEASE(dirty) if (dirty != NULL) { dirty->Release(); dirty = NULL; }

IMMDeviceEnumerator *pEnumerator = NULL;
IMMDevice *pDevice = NULL;
IAudioClient *pAudioClient = NULL;
IAudioCaptureClient *pCaptureClient = NULL;
WAVEFORMATEX *outWFX = NULL;

// List devices
IMMDeviceCollection *pCollection = NULL;
IMMDevice *pEndpoint = NULL;
IPropertyStore *pProps = NULL;
LPWSTR pwszID = NULL;


bool AudioOut::setOutputDevice(int index)
{
	IPropertyStore *pProps = NULL;

	size_t REFTIMES_PER_SEC = 400000;

	HRESULT hr;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	REFERENCE_TIME hnsActualDuration;
	UINT32 bufferFrameCount;

	releaseDevices();
	releaseDeviceCollection();

	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL, 
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);
	if (releaseDevices(hr)) { return false; }


	hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
	if (releaseDeviceCollection(hr)) { releaseDevices(); return false; }

	UINT count;
	hr = pCollection->GetCount(&count);
	if (releaseDeviceCollection(hr)) { releaseDevices(); return false; }

	if (index >= count)
	{
		releaseDeviceCollection();
		releaseDevices();
		return false;
	}

	hr = pCollection->Item(index, &pDevice);
	if (releaseDevices(hr))
	{
		hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
		if (releaseDevices(hr)) { releaseDeviceCollection(); return false; }
	}
	
	hr = pDevice->Activate(
		IID_IAudioClient, CLSCTX_ALL,
		NULL, (void**)&pAudioClient);
	if (releaseDevices(hr)) { releaseDeviceCollection(); return false; }

	// Print name
	hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
	if (FAILED(hr)) { SAFE_RELEASE(pProps); }
	else
	{
		PROPVARIANT varName;
		PropVariantInit(&varName);
		hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
		if (!FAILED(hr))
		{
			printf("Output device: \"%S\"\n", varName.pwszVal);
		}
	}

	hr = pAudioClient->GetMixFormat(&outWFX);
	if (releaseDevices(hr)) { releaseDeviceCollection(); return false; }

	hr = pAudioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		AUDCLNT_STREAMFLAGS_LOOPBACK,
		hnsRequestedDuration,
		0,
		outWFX,
		NULL);
	if (releaseDevices(hr)) { releaseDeviceCollection(); return false; }

	// Get the size of the allocated buffer.
	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	if (releaseDevices(hr)) { releaseDeviceCollection(); return false; }

	hr = pAudioClient->GetService(
		IID_IAudioCaptureClient,
		(void**)&pCaptureClient);
	if (releaseDevices(hr)) { releaseDeviceCollection(); return false; }

	// Calculate the actual duration of the allocated buffer.
	hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / outWFX->nSamplesPerSec;

	hr = pAudioClient->Start();  // Start recording.
	if (releaseDevices(hr)) { releaseDeviceCollection(); return false; }

	maxOutBufferSize = AUDIO_OUT_SAMPLES_PER_BUFFER * outWFX->nChannels;

	return true;
}

const std::vector<AudioOutDevice> AudioOut::getDevices()
{
	return _devices;
}

void AudioOut::captureAudio()
{
	HRESULT hr;
	BYTE *pData;
	UINT32 numFramesInPacket = 0;
	UINT32 numFramesAvailable;
	DWORD flags;

	if (pCaptureClient == NULL) { setOutputDevice(); return; }

	hr = pCaptureClient->GetNextPacketSize(&numFramesInPacket);
	if (releaseDevices(hr)) { return; }

	while (numFramesInPacket != 0)
	{
		// Get the available data in the shared buffer.
		hr = pCaptureClient->GetBuffer(
			&pData,
			&numFramesAvailable,
			&flags, NULL, NULL
		);
		if (releaseDevices(hr)) { return; }

		if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
		{
			pData = NULL;  // Tell CopyData to write silence.
		}

		if (pData != NULL)
		{
			if (_outBuffers[_activeBuffer].size() + numFramesInPacket >= maxOutBufferSize)
			{
				size_t framesToFill = maxOutBufferSize - _outBuffers[_activeBuffer].size();
				size_t leftoverFrames = numFramesAvailable - framesToFill;

				float sampleF;
				int16_t sampleI;
				byte *p = pData;
				byte *pe = p + framesToFill * AUDIO_OUT_CHANNELS * AUDIO_OUT_BYTES_PER_SAMPLE;
				for (; p < pe; p += 4)
				{
					sampleF = *(float*)p;
					sampleI = max(min(sampleF, 1), -1) * 32767.0f;
					_outBuffers[_activeBuffer].push_back( isEnabled ? (sampleI * volume) : 0 );
				}

				swapBuffers();
				_outBuffers[_activeBuffer].clear();
				_isReady = true;

				pe = p + leftoverFrames * AUDIO_OUT_CHANNELS * AUDIO_OUT_BYTES_PER_SAMPLE;
				for (; p < pe; p += 4)
				{
					sampleF = *(float*)p;
					sampleI = max(min(sampleF, 1), -1) * 32767.0f;
					_outBuffers[_activeBuffer].push_back( isEnabled ? (sampleI * volume) : 0 );
				}
			}
			else
			{
				float sampleF;
				int16_t sampleI;
				byte *p = pData;
				byte *pe = p + numFramesInPacket * AUDIO_OUT_CHANNELS * AUDIO_OUT_BYTES_PER_SAMPLE;
				for (; p < pe; p += 4)
				{
					sampleF = *(float*)p;
					sampleI = max(min(sampleF, 1), -1) * 32767.0f;
					_outBuffers[_activeBuffer].push_back( isEnabled ? (sampleI * volume) : 0 );
				}
			}
		}

		if (releaseDevices(hr)) { return; }

		hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
		if (releaseDevices(hr)) { return; }

		hr = pCaptureClient->GetNextPacketSize(&numFramesInPacket);
		if (releaseDevices(hr)) { return; }
	}
}

bool AudioOut::isReady()
{
	return _isReady;
}

const std::vector<int16_t> AudioOut::popBuffer()
{
	if (!_isReady)
	{
		return std::vector<int16_t>();
	}
	
	_isReady = false;
	return std::vector<int16_t>(_outBuffers[1-_activeBuffer]);
}

const int AudioOut::popPreviewDecibel()
{
	static int inactiveBuffer;
	inactiveBuffer = 1 - _activeBuffer;

	if (_previewIndex >= 0 && _previewIndex < _outBuffers[inactiveBuffer].size())
	{
		static int decibelValue;
		decibelValue = AudioTools::previewDecibel(_outBuffers[inactiveBuffer][_previewIndex]);
		_previewIndex++;
		
		return decibelValue;
	}

	return AUDIOTOOLS_PREVIEW_MIN_DB;
}

const uint32_t AudioOut::getFrequency() const
{
	if (outWFX != NULL)
	{
		return outWFX->nSamplesPerSec;
	}
	return AUDIO_OUT_DEFAULT_FREQUENCY;
}

// TODO Better check if the release here interferes with capture loop
void AudioOut::fetchDevices()
{
	HRESULT hr;

	releaseDeviceCollection();
	
	CoInitialize(nullptr);
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator
	);
	if (releaseDeviceCollection(hr)) { return; }

	hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
	if (releaseDeviceCollection(hr)) { return; }

	UINT count;
	hr = pCollection->GetCount(&count);
	if (releaseDeviceCollection(hr)) { return; }

	_devices.clear();
	if (count == 0) { std::cerr << "No endpoints found!" << std::endl; }
	for (size_t i = 0; i < count; i++)
	{
		hr = pCollection->Item(i, &pEndpoint);
		if (releaseDeviceCollection(hr)) { return; }

		hr = pEndpoint->GetId(&pwszID);
		if (releaseDeviceCollection(hr)) { return; }

		hr = pEndpoint->OpenPropertyStore(STGM_READ, &pProps);
		if (releaseDeviceCollection(hr)) { return; }

		PROPVARIANT varName;
		PropVariantInit(&varName);
		hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
		if (releaseDeviceCollection(hr)) { return; }

		std::wstring wname(varName.pwszVal);
		std::wstring wid(pwszID);
		std::string name(wname.begin(), wname.end());
		std::string id(wid.begin(), wid.end());
		printf("Endpoint %d: \"%S\" (%S)\n",i, varName.pwszVal, pwszID);
		_devices.push_back({ name, id, i });

		CoTaskMemFree(pwszID);
		pwszID = NULL;
		PropVariantClear(&varName);

		SAFE_RELEASE(pProps);
		SAFE_RELEASE(pEndpoint);
	}

	SAFE_RELEASE(pEnumerator);
	SAFE_RELEASE(pCollection);
	return;
}


// ====================================================
//   PRIVATE
// ====================================================
void AudioOut::swapBuffers()
{
	_activeBuffer = 1 - _activeBuffer;
	_previewIndex = 0;
}

bool AudioOut::releaseDevices(HRESULT hr)
{
	if (FAILED(hr))
	{
		CoTaskMemFree(outWFX);
		SAFE_RELEASE(pEnumerator);
		SAFE_RELEASE(pDevice);
		SAFE_RELEASE(pAudioClient);
		SAFE_RELEASE(pCaptureClient);
		return true;
	}

	return false;
}

bool AudioOut::releaseDeviceCollection(HRESULT hr)
{
	if (FAILED(hr))
	{
		CoTaskMemFree(pwszID);
		SAFE_RELEASE(pEnumerator);	// TODO Check later if this interferes with capture loop
		SAFE_RELEASE(pCollection);
		SAFE_RELEASE(pEndpoint);
		SAFE_RELEASE(pProps);
		return true;
	}
	return false;
}
