#pragma once

#include <d3d11.h>
#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppColors.h"
#include "IconButton.h"

class ToggleIconButtonWidget
{
public:
	static bool render(
		Icon onIcon, Icon offIcon, bool isOn, const char* id,
		ImVec4 tintOn = AppColors::positive, ImVec4 tintOff = AppColors::negative,
		ImVec2 size = ICONBUTTON_SIZE
	);

	static bool render(
		Icon onIcon, Icon offIcon, bool isOn,
		ImVec4 tintOn = AppColors::positive, ImVec4 tintOff = AppColors::negative,
		ImVec2 size = ICONBUTTON_SIZE
	);

	static bool render(Icon onIcon, Icon offIcon, bool isOn, const char* id, ImVec2 size);

	static bool render(Icon onIcon, Icon offIcon, bool isOn, ImVec2 size);
};