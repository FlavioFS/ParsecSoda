#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppStyle.h"
#include "../Stopwatch.h"
#include "../Hosting.h"
#include <string>
#include "TextCenteredWidget.h"
#include "IconButton.h"
#include "BoolButtonWidget.h"
#include "ToggleIconButtonWidget.h"
#include "TitleTooltipWidget.h"
#include "IntRangeWidget.h"
#include "GradientProgressBar.h"
#include "BadgeIconWidget.h"
#include "BadgeButtonWidget.h"

using namespace std;

class HotseatWidget
{
public:
	static void render(Hosting& hosting);

private:
	static void renderTopBar(HotseatManager& hotseatManager, bool& isWindowLocked);
	static void renderTimePickers(HotseatManager& hotseatManager);
	static void renderHotseats(HotseatManager& hotseatManager);
	static void renderWaitingGuests(HotseatManager& hotseatManager);
	static void renderDummy(float x = 2, float y = 2);
	static void renderSeparator();
};

