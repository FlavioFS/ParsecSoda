#include "AudioSource.h"

// ==================================================
//   Constants
// ==================================================
#define AUDIOSRC_BITS 32
#define AUDIOSRC_BYTES_PER_SAMPLE (AUDIOSRC_BITS/8)


// ==================================================
//   MACROS
// ==================================================
#define SAFE_RELEASE(dirty) if (dirty != nullptr) { dirty->Release(); dirty = nullptr; }
#define FAIL_EXIT(HR) if(FAILED(HR)) goto EXIT;


bool AudioSource::setDevice(int index)
{
	HRESULT hr;
	size_t REFTIMES_PER_SEC = 400000;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	REFERENCE_TIME hnsActualDuration;
	UINT32 bufferFrameCount;

	IMMDeviceEnumerator* pEnumerator = nullptr;
	IMMDeviceCollection* pCollection = nullptr;
	IMMDevice* pDevice = nullptr;
	IAudioClient* pAudioClient = nullptr;
	WAVEFORMATEX* pwfx = nullptr;
	LPWSTR pwszID = nullptr;
	IPropertyStore* pProps = nullptr;

	uint32_t freq = 1000;

	auto releaseLocal = [&]()
	{
		CoTaskMemFree(pwszID);
		CoTaskMemFree(pwfx);
		SAFE_RELEASE(pEnumerator);
		SAFE_RELEASE(pCollection);
		SAFE_RELEASE(pDevice);
		SAFE_RELEASE(pAudioClient);
		SAFE_RELEASE(pProps);
	};

	auto releaseAll = [&]() {
		releaseLocal();
		SAFE_RELEASE(m_pCaptureClient);
	};


	m_mutex.lock();

	m_buffers[0].clear();
	m_buffers[1].clear();

	releaseAll();

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	hr = CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
	FAIL_EXIT(hr);

	hr = pEnumerator->EnumAudioEndpoints(m_eDataFlow, DEVICE_STATE_ACTIVE, &pCollection);
	FAIL_EXIT(hr);

	UINT count;
	hr = pCollection->GetCount(&count);
	FAIL_EXIT(hr);
	if (index >= count) goto EXIT;

	hr = pCollection->Item(index, &pDevice);
	if (FAILED(hr))
	{
		releaseAll();
		hr = pEnumerator->GetDefaultAudioEndpoint(m_eDataFlow, eConsole, &pDevice);
		FAIL_EXIT(hr);
	}

	hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
	FAIL_EXIT(hr);

	hr = pAudioClient->GetMixFormat(&pwfx);
	FAIL_EXIT(hr);

	freq = (uint32_t)pwfx->nSamplesPerSec;
	setFrequency((uint32_t) pwfx->nSamplesPerSec);

	hr = pAudioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		m_streamFlags,
		hnsRequestedDuration,
		0,
		pwfx,
		NULL);
	FAIL_EXIT(hr);

	// Gets the size of the allocated buffer.
	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	FAIL_EXIT(hr);

	hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&m_pCaptureClient);
	FAIL_EXIT(hr);

	// Calculates the actual duration of the allocated buffer.
	hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / pwfx->nSamplesPerSec;

	// Starts recording.
	hr = pAudioClient->Start();
	FAIL_EXIT(hr);

	m_channels = (size_t)pwfx->nChannels;
	m_maxBufferSize = AUDIOSRC_SAMPLES_PER_BUFFER * m_channels;

	currentDevice = m_devices[index];

	releaseLocal();

	m_mutex.unlock();
	return true;

EXIT:
	releaseAll();
	m_mutex.unlock();
	return false;
}

const std::vector<AudioSourceDevice> AudioSource::getDevices()
{
	return m_devices;
}

void AudioSource::captureAudio()
{
	if (!isEnabled) return;

	m_mutex.lock();

	HRESULT hr;
	BYTE* pData;
	UINT32 numFramesInPacket = 0;
	UINT32 numFramesAvailable;
	DWORD flags;

	if (m_pCaptureClient == NULL) {
		m_mutex.unlock();
		setDevice();
		return;
	}

	hr = m_pCaptureClient->GetNextPacketSize(&numFramesInPacket);
	FAIL_EXIT(hr);

	while (numFramesInPacket != 0)
	{
		// Gets the available data in the shared buffer.
		hr = m_pCaptureClient->GetBuffer(
			&pData,
			&numFramesAvailable,
			&flags, NULL, NULL
		);
		FAIL_EXIT(hr);

		if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
		{
			pData = NULL;  // Tells CopyData to write silence.
		}

		if (pData != NULL)
		{
			size_t frameCount = numFramesAvailable;
			size_t requiredBufferLength = m_channels * frameCount;
			size_t currentBufferSize = (m_channels >= 2) ? m_buffers[m_activeBuffer].size() : (m_buffers[m_activeBuffer].size() >> 1); // Mirror channel. Mono -> Stereo

			if (currentBufferSize + requiredBufferLength >= m_maxBufferSize)
			{
				size_t bufferLengthToFill = m_maxBufferSize - currentBufferSize;
				size_t bufferLengthLeftover = requiredBufferLength - bufferLengthToFill;

				float sampleF;
				int16_t sampleI;
				int16_t finalSample;
				BYTE* p = pData;
				BYTE* pe = p + bufferLengthToFill * AUDIOSRC_BYTES_PER_SAMPLE;
				for (; p < pe; p += 4)
				{
					sampleF = *(float*)p;
					sampleI = max(min(sampleF, 1), -1) * 32767.0f;
					finalSample = isEnabled ? (sampleI * volume) : 0;
					m_buffers[m_activeBuffer].push_back(finalSample);

					if (m_channels < 2)
					{
						m_buffers[m_activeBuffer].push_back(finalSample); // Mirror channel. Mono -> Stereo
					}
				}

				swapBuffers();
				m_buffers[m_activeBuffer].clear();
				m_isReady = true;

				pe = p + bufferLengthLeftover * AUDIOSRC_BYTES_PER_SAMPLE;
				for (; p < pe; p += 4)
				{
					sampleF = *(float*)p;
					sampleI = max(min(sampleF, 1), -1) * 32767.0f;
					finalSample = isEnabled ? (sampleI * volume) : 0;
					m_buffers[m_activeBuffer].push_back(finalSample);

					if (m_channels < 2)
					{
						m_buffers[m_activeBuffer].push_back(finalSample); // Mirror channel. Mono -> Stereo
					}
				}
			}
			else
			{
				float sampleF;
				int16_t sampleI;
				int16_t finalSample;
				BYTE* p = pData;
				BYTE* pe = p + requiredBufferLength * AUDIOSRC_BYTES_PER_SAMPLE;
				for (; p < pe; p += 4)
				{
					sampleF = *(float*)p;
					sampleI = max(min(sampleF, 1), -1) * 32767.0f;
					finalSample = isEnabled ? (sampleI * volume) : 0;
					m_buffers[m_activeBuffer].push_back(finalSample);

					if (m_channels < 2)
					{
						m_buffers[m_activeBuffer].push_back(finalSample); // Mirror channel. Mono -> Stereo
					}
				}
			}

			if (m_isPlotActive)
			{
				for (size_t i = 0; i < m_buffers[m_activeBuffer].size() >> 1; i += 2)
				{
					m_plotBuffer[i] = 0.5f * ((float)m_buffers[m_activeBuffer][i << 1] + (float)m_buffers[m_activeBuffer][(i << 1) + 1]);
				}
			}
		}

		FAIL_EXIT(hr);

		hr = m_pCaptureClient->ReleaseBuffer(numFramesAvailable);
		FAIL_EXIT(hr);

		hr = m_pCaptureClient->GetNextPacketSize(&numFramesInPacket);
		FAIL_EXIT(hr);

	}

	m_mutex.unlock();
	return;

EXIT:
	SAFE_RELEASE(m_pCaptureClient);
	m_mutex.unlock();
}

bool AudioSource::isReady()
{
	return m_isReady;
}

const std::vector<int16_t> AudioSource::popBuffer()
{
	if (!m_isReady)
	{
		return std::vector<int16_t>();
	}

	m_isReady = false;
	return std::vector<int16_t>(m_buffers[1 - m_activeBuffer]);
}

const int AudioSource::popPreviewDecibel()
{
	int inactiveBuffer;
	inactiveBuffer = 1 - m_activeBuffer;

	if (m_previewIndex >= 0 && m_previewIndex < m_buffers[inactiveBuffer].size())
	{
		int decibelValue;
		decibelValue = AudioTools::previewDecibel(isEnabled ? m_buffers[inactiveBuffer][m_previewIndex] : 0);
		m_previewIndex++;

		return decibelValue;
	}

	return AUDIOTOOLS_PREVIEW_MIN_DB;
}

const float* AudioSource::getPlot()
{
	return m_plotBuffer;
}

const void AudioSource::togglePlot(const bool enabled)
{
	m_isPlotActive = enabled;
}

// TODO Better check if the release here interferes with capture loop
void AudioSource::fetchDevices()
{
	IMMDeviceEnumerator* pEnumerator = nullptr;
	IMMDeviceCollection* pCollection = nullptr;
	IMMDevice* pEndpoint = nullptr;
	IPropertyStore* pProps = nullptr;
	LPWSTR pwszID = nullptr;

	HRESULT hr;

	auto releaseAll = [&]() {
		CoTaskMemFree(pwszID);
		SAFE_RELEASE(pEnumerator);
		SAFE_RELEASE(pCollection);
		SAFE_RELEASE(pEndpoint);
		SAFE_RELEASE(pProps);
	};

	releaseAll();

	CoInitialize(nullptr);
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator
	);
	FAIL_EXIT(hr);

	hr = pEnumerator->EnumAudioEndpoints(m_eDataFlow, DEVICE_STATE_ACTIVE, &pCollection);
	FAIL_EXIT(hr);

	UINT count;
	hr = pCollection->GetCount(&count);
	FAIL_EXIT(hr);

	m_devices.clear();
	if (count == 0) { std::cerr << "No endpoints found!" << std::endl; }
	for (size_t i = 0; i < count; i++)
	{
		hr = pCollection->Item(i, &pEndpoint);
		FAIL_EXIT(hr);

		hr = pEndpoint->GetId(&pwszID);
		FAIL_EXIT(hr);

		hr = pEndpoint->OpenPropertyStore(STGM_READ, &pProps);
		FAIL_EXIT(hr);

		PROPVARIANT varName;
		PropVariantInit(&varName);
		hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
		FAIL_EXIT(hr);

		std::wstring wname(varName.pwszVal);
		std::wstring wid(pwszID);
		std::string name(wname.begin(), wname.end());
		std::string id(wid.begin(), wid.end());
		printf("Endpoint %d: \"%S\" (%S)\n", i, varName.pwszVal, pwszID);
		m_devices.push_back({ name, id, i });

		CoTaskMemFree(pwszID);
		pwszID = NULL;
		PropVariantClear(&varName);

		SAFE_RELEASE(pProps);
		SAFE_RELEASE(pEndpoint);
	}

EXIT:
	releaseAll();

	return;
}

void AudioSource::setFrequency(uint32_t  frequency)
{
	m_frequency = frequency;
}

const uint32_t AudioSource::getFrequency() const
{
	return m_frequency;
}


// ====================================================
//   PRIVATE
// ====================================================
void AudioSource::swapBuffers()
{
	m_activeBuffer = 1 - m_activeBuffer;
	m_previewIndex = 0;
}

void AudioSource::setEDataFlow(EDataFlow eDataFlow)
{
	m_eDataFlow = eDataFlow;
}

void AudioSource::setStreamFlags(DWORD streamFlags)
{
	m_streamFlags = streamFlags;
}
