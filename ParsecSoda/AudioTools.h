#pragma once

#include <iostream>

#define AUDIOTOOLS_PREVIEW_MIN_DB -60
#define AUDIOTOOLS_PREVIEW_MAX_AMP 32768
#define AUDIOTOOLS_PREVIEW_MIN_AMP 0.001f

using namespace std;

class AudioTools
{
public:
	static const int previewDecibel(int16_t amplitude)
	{
		static int decibelValue;
		static float relativeAmp, relativeAmpClamped;

		relativeAmp = abs(((float)amplitude) / ((float)AUDIOTOOLS_PREVIEW_MAX_AMP));
		relativeAmpClamped = max(AUDIOTOOLS_PREVIEW_MIN_AMP, relativeAmp);
		decibelValue = (int)(20.0f * log10(relativeAmpClamped));

		return max(AUDIOTOOLS_PREVIEW_MIN_DB, decibelValue);
	}

	static const float decibelToFloat(int decibel)
	{
		static const int absMinDb = abs(AUDIOTOOLS_PREVIEW_MIN_DB);
		return ((float)(decibel + absMinDb)) / ((float)absMinDb);
	}
};

