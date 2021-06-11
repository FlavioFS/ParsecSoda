#pragma once

#include "IWidget.h"
#include "../imgui/imgui.h"
#include "../AppStyle.h"

class TooltipWidget
{
public:
	static bool render(const char* text, AppStyle &style);
};

