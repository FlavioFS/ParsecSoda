#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppStyle.h"

class TooltipWidget
{
public:
	static bool render(const char* text, bool forceShow = false);
};

