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
	static void renderBadgeIcon(
		Icon icon, const char* tooltipTitle, const char* tooltipDescription,
		ImVec4 tint = AppColors::primary, ImVec2 size = ImVec2(25, 25)
	);
	static bool renderBadgeButton(
		Icon icon, const char* tooltipTitle, const char* tooltipDescription,
		const char* id = nullptr, ImVec2 size = ImVec2(25, 25), ImVec4 tint = AppColors::primary
	);
	static void renderDummy(float x = 2, float y = 2);
	static void renderSeparator();
};

