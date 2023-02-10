#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "../Stopwatch.h"
#include "ToggleIconButtonWidget.h"
#include "TitleTooltipWidget.h"
#include "AnimatedGamepadWidget.h"
#include "LoadingRingWidget.h"
#include "IntRangeWidget.h"
#include "LoadingRingWidget.h"
#include "BadgeIconWidget.h"
#include "BadgeButtonWidget.h"
#include "SwitchWidget.h"

class GamepadsWidget
{
public:
	GamepadsWidget(Hosting& hosting);
	bool render();

private:
	void renderTopBar(bool& isWindowLocked, const ImVec2& windowSize);
	void renderOptionsMenu();
	void renderPadInputTypeIcon(AGamepad* pad);

	// Dependency injection
	Hosting& _hosting;
	
	// Attributes
	string _logBuffer;
	vector<AGamepad*>& _gamepads;
};