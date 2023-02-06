#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppStyle.h"

class GradientProgressBar
{
public:
	static void renderSingle(
		const float fraction, const ImVec2& size = ImVec2(200, 30),
		const char* overlay = "",
		const ImVec4 colorFinal = AppColors::positive
	);

	static void renderGradient(
		const float fraction, const ImVec2& size = ImVec2(200, 30),
		const char* overlay = "",
		const ImVec4 colorFull = AppColors::positive,
		const ImVec4 colorHalf = AppColors::warning2,
		const ImVec4 colorEmpty = AppColors::negative
	);

	static ImVec4 lerpColors3(float t, ImVec4 colorFull = AppColors::positive, ImVec4 colorHalf = AppColors::warning2, ImVec4 colorEmpty = AppColors::negative)
	{
		if (t >= 0.5f)
		{
			return lerpColors2(2.0f * (t - 0.5f), colorFull, colorHalf);
		}
		else
		{
			return lerpColors2(2.0f * t, colorHalf, colorEmpty);
		}
	}
private:

	static ImVec4 lerpColors2(float t, ImVec4 colorFull, ImVec4 colorEmpty)
	{
		return colorFull * t + colorEmpty * (1 - t);
	}
};

