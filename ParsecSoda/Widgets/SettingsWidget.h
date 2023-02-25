#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppStyle.h"
#include "../MetadataCache.h"
#include "SwitchWidget.h"
#include "BadgeButtonWidget.h"
#include "../SFXList.h"

class SettingsWidget
{
public:
	static bool render(SFXList& sfxList);
};

