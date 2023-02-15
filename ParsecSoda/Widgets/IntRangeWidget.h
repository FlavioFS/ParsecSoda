#pragma once

#include "../imgui/imgui.h"
#include "../imgui/ImVecExtensions.h"
#include "../globals/AppStyle.h"
#include "IconButton.h"
#include <string>
#include <functional>

using namespace std;

class IntRangeWidget
{
public:
	typedef function<void(void)> Action;

	template<typename T>
	static bool render(const char* id, T& value, T minimum = 0, T maximum = 100, IntRangeWidget::Action releaseDragCallback = nullptr, float speed = 0.05f, float width = 60.0f);
};