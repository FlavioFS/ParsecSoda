#pragma once

#include <iostream>
#include <functional>
#include "../imgui/imgui.h"
#include "../globals/AppStyle.h"
#include "../globals/AppIcons.h"
#include "ToggleIconButtonWidget.h"

using namespace std;

class AudioControlWidget
{
public:
	typedef function<void(void)> Action;

	static bool render(
		const char* id, int* volume, bool isEnabled, float preview = 0.0f,
		Icon iconOn = AppIcons::speakersOn, Icon iconOff = AppIcons::speakersOff,
		AudioControlWidget::Action releaseVolumeCallback = nullptr
	);
};

