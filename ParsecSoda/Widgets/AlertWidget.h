#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"

class AlertWidget
{
public:
	static bool render(const char* title, const char* message);
};

