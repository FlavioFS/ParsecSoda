#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppStyle.h"
#include "../globals/AppIcons.h"
#include "ToggleIconButtonWidget.h"

class BoolButtonWidget
{
public:
	static bool render(const char* label, bool& value);
};