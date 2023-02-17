#pragma once

#include "../imgui/imgui.h"
#include "../imgui/ImVecExtensions.h"
#include "../MetadataCache.h"
#include "../globals/AppStyle.h"
#include "../globals/AppIcons.h"
#include "../ButtonLock.h"
#include "../Stringer.h"
#include "../Mouse.h"
#include "TitleTooltipWidget.h"
#include "BadgeButtonWidget.h"
#include "SwitchWidget.h"
#include <string>
#include "ViGEm/Common.h"

using namespace std;

class ButtonLockWidget
{
public:
	static bool render();

private:
	static void renderGamepad(const float& height);


	static void renderBoolTextureButton(
		bool& state,
		const string& id, const Icon texture, const ImVec2& size,
		const char* tooltipTitleTrue, const char* tooltipTitleFalse,
		const char* tooltipDescriptionTrue, const char* tooltipDescriptionFalse,
		const string& label = "", const bool& flipX = false, const bool& flipY = false
	);

	static void renderBitmaskTextureButton(
		const unsigned int& bitMask,
		const string& id, const Icon texture, const ImVec2& size,
		const char* tooltipTitleTrue, const char* tooltipTitleFalse,
		const char* tooltipDescriptionTrue, const char* tooltipDescriptionFalse,
		const string& label = "", const bool& flipX = false, const bool& flipY = false
	);

	static bool renderToggleTextureButton(
		bool& state,
		const string& id, const Icon texture, const ImVec2& size,
		const char* tooltipTitleTrue, const char* tooltipTitleFalse,
		const char* tooltipDescriptionTrue, const char* tooltipDescriptionFalse,
		const string& label = "", const bool& flipX = false, const bool& flipY = false
	);

	static void renderAnalog(
		bool& stick, const unsigned short& thumbBitMask,
		const string& buttonName, const float& stickRadius, const float& thumbRadius
	);
};

