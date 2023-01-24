#pragma once

#include "../CircularList.h"
#include "../imgui/implot.h"
#include "../globals/AppColors.h"

class CircularListWidget
{
	template <typename T>
	static void render(const char* titleID, const char* labelID, CircularList<T>& circularList, float padding = 10, const char* tagNumberFormat = "%4.0f");
};