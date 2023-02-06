#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppStyle.h"

class GradientProgressBar
{
public:
	static void render(
		const float fraction, const ImVec2& size = ImVec2(200, 30),
		const char* overlay = "",
		const ImVec4 colorFull = AppColors::positive,
		const ImVec4 colorHalf = AppColors::warning2,
		const ImVec4 colorEmpty = AppColors::negative
	);

private:
	inline static ImVec4 lerpColors3(ImVec4 colorFull, ImVec4 colorHalf, ImVec4 colorEmpty, float t)
	{
		if (t >= 0.5f)
		{
			return lerpColors2(colorFull, colorHalf, 2.0f * (t - 0.5f));
		}
		else
		{
			return lerpColors2(colorHalf, colorEmpty, 2.0f * t);
		}
	}

	inline static ImVec4 lerpColors2(ImVec4 colorFull, ImVec4 colorEmpty, float t)
	{
		return colorFull * t + colorEmpty * (1 - t);
	}
};

