#include "AudioIn.h"

// ==================================================
//   Input Sources
// ==================================================
#define AUDIO_IN_FREQUENCY_HZ 44100
//#define AUDIO_IN_CHANNELS 2
//#define AUDIO_IN_SWAP_BUFFERS 2
//
//#define AUDIO_IN_BITS 16
//#define AUDIO_IN_BYTES (AUDIO_IN_BITS/8)
//#define AUDIO_IN_SAMPLE_COUNT (4 * AUDIO_IN_FREQUENCY_HZ / 100)								// 1764 samples
//#define AUDIO_IN_BUFFER_SIZE (AUDIO_IN_SAMPLE_COUNT * AUDIO_IN_CHANNELS * AUDIO_IN_BYTES)

//#define AUDIO_IN_FREQUENCY_HZ 44100

#define AUDIO_IN_SWAP_BUFFERS 2
#define AUDIO_IN_CHANNELS 2
#define AUDIO_IN_BITS 16

//#define AUDIO_IN_SAMPLE_COUNT (4 * AUDIO_IN_FREQUENCY_HZ / 100)								// 1764 samples
//#define AUDIO_IN_BUFFER_SIZE (AUDIO_IN_SAMPLE_COUNT * AUDIO_IN_CHANNELS * AUDIO_IN_BYTES)	// 7056 bytes = 1764 samples x 2 channels x 2 bytes/sample

/**
* Buffer Size explained.
* 44100 Hz / 100  == 441 == 3² * 7²
* 48000 Hz / 1000 == 48  == 2^4 * 3
* GCD(441, 48) == 2^4 * 3² * 7² == 7056
* This is probably the best common ground.
*/
#define AUDIO_IN_BUFFER_SIZE 7056


HWAVEIN _win;
WAVEFORMATEX _wfx = {};
WAVEHDR _headers[AUDIO_IN_SWAP_BUFFERS] = { {},{} };
char _buffers[AUDIO_IN_SWAP_BUFFERS][AUDIO_IN_BUFFER_SIZE];
std::vector<int16_t> _inBuffer;



// ==================================================
//   Output Sources
// ==================================================
void AudioIn::setFrequency(Frequency frequency)
{
	_frequency = (uint32_t) frequency;
	_wfx.nSamplesPerSec = _frequency;
	MetadataCache::preferences.micFrequency = _frequency;
	MetadataCache::savePreferences();
}

Frequency AudioIn::getFrequency()
{
	return (Frequency)_frequency;
}

bool AudioIn::init(AudioInDevice device)
{
	if (_win != nullptr)
	{
		waveInStop(_win);
		for (auto& h : _headers)
		{
			waveInUnprepareHeader(_win, &h, sizeof(h));
		}
		waveInClose(_win);
	}
	
	_wfx.wFormatTag = WAVE_FORMAT_PCM;
	_wfx.nChannels = AUDIO_IN_CHANNELS;
	_wfx.nSamplesPerSec = _frequency;
	_wfx.wBitsPerSample = AUDIO_IN_BITS;
	_wfx.nBlockAlign = (_wfx.wBitsPerSample / 8) * _wfx.nChannels;
	_wfx.nAvgBytesPerSec = _wfx.nBlockAlign * _wfx.nSamplesPerSec;

	MMRESULT result;
	result = waveInOpen(
		&_win,
		device.isEmpty ? WAVE_MAPPER : device.id,
		&_wfx, NULL, NULL, CALLBACK_NULL | WAVE_FORMAT_DIRECT
	);
	if (result != 0)
	{
		return false;
	}

	_device = device;

	for (size_t i = 0; i < _wfx.nChannels; i++)
	{
		_headers[i].lpData = _buffers[i];
		_headers[i].dwBufferLength = AUDIO_IN_BUFFER_SIZE;

		waveInPrepareHeader(_win, &_headers[i], sizeof(_headers[i]));
		waveInAddBuffer(_win, &_headers[i], sizeof(_headers[i]));
	}

	waveInStart(_win);
	return true;
}

void AudioIn::reinit(Frequency frequency)
{
	_mutex.lock();
	if (!_device.isEmpty)
	{
		setFrequency(frequency);
		init(_device);
	}
	_mutex.unlock();
}

void AudioIn::captureAudio()
{
	_mutex.lock();

	try
	{
		for (size_t i = 0; i < AUDIO_IN_SWAP_BUFFERS; i++)
		{
			if (_headers[i].dwFlags & WHDR_DONE)
			{
				_inBuffer.clear();
				byte *p = (byte*)&_buffers[i];
				byte *pe = p + AUDIO_IN_BUFFER_SIZE;
				for (; p < pe; p+=2)
				{
					_inBuffer.push_back(
						isEnabled ? (*(int16_t*)p * volume) : 0
					);
				}

				_isReady = true;
				_activeBuffer = i;
				_previewIndex = 0;

				_headers[i].dwFlags = 0;
				_headers[i].dwBytesRecorded = 0;

				waveInPrepareHeader(_win, &_headers[i], sizeof(_headers[i]));
				waveInAddBuffer(_win, &_headers[i], sizeof(_headers[i]));
			}
		}
	}
	catch (const std::exception&)
	{
		// Just in case
		std::cerr << "AudioIn failed to capture!" << std::endl;
	}

	_mutex.unlock();
}

const bool AudioIn::isReady() const
{
	return _isReady;
}

const std::vector<int16_t> AudioIn::popBuffer()
{
	if (_isReady)
	{
		_isReady = false;
		return _inBuffer;
	}

	return std::vector<int16_t>();
}

const int AudioIn::popPreviewDecibel()
{
	if (_previewIndex >= 0 && _previewIndex < _inBuffer.size())
	{
		static int decibelValue;
		decibelValue = AudioTools::previewDecibel(isEnabled ? _inBuffer[_previewIndex] : 0);
		_previewIndex++;
		
		return decibelValue;
	}

	return AUDIOTOOLS_PREVIEW_MIN_DB;
}

const std::vector<AudioInDevice> AudioIn::listInputDevices() const
{
	WAVEINCAPS wave;
	std::vector<AudioInDevice> devices;

	int deviceCount = waveInGetNumDevs();

	for (UINT i = 0; i < deviceCount; i++)
	{
		if (!waveInGetDevCaps(i, &wave, sizeof(WAVEINCAPS)))
		{
			AudioInDevice dev;
			dev.isEmpty = false;
			dev.wave = wave;
			dev.id = i;

			wstring wname = wave.szPname;
			dev.name = string(wname.begin(), wname.end());
			
			devices.push_back(dev);
		}
	}

	return devices;
}

AudioInDevice AudioIn::selectInputDevice(const int index)
{
	_mutex.lock();

	std::vector<AudioInDevice>devices = listInputDevices();

	if (devices.empty() || index < 0 || index >= devices.size() ) {
		_mutex.unlock();
		return AudioInDevice();
	}

	currentDevice = devices[index];
	currentDevice.isEmpty = false;

	init(currentDevice);

	_mutex.unlock();
	return currentDevice;
}