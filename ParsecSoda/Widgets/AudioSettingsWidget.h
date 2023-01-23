#pragma once

#include "../imgui/imgui.h"
#include "../imgui/implot.h"
#include "../Hosting.h"
#include "../AudioTools.h"
#include "../globals/AppIcons.h"
#include "../globals/AppFonts.h"
#include "../globals/AppColors.h"
#include "../globals/AppStyle.h"
#include "ToggleIconButtonWidget.h"
#include "TitleTooltipWidget.h"
#include "TooltipWidget.h"
#include "ConfirmPopupWidget.h"
#include "IconButton.h"
#include "IntRangeWidget.h"
#include "BoolButtonWidget.h"
#include "AudioControlWidget.h"
#include "../MetadataCache.h"

class AudioSettingsWidget
{
public:
	AudioSettingsWidget(Hosting& hosting);
	bool render();
	
	const ImVec2 DEFAULT_BUTTON_SIZE = ImVec2(40, 40);

private:
	float lerp(float val1, float val2, float t = 0.11f);
	float easing(float t);

	// Dependency injection
	Hosting& _hosting;
	AudioIn& _audioIn;
	AudioOut& _audioOut;

	vector<AudioSourceDevice> _inputs;
	vector<AudioSourceDevice> _outputs;
};