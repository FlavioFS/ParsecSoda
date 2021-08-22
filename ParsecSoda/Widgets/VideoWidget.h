#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppIcons.h"
#include "../globals/AppFonts.h"
#include "../globals/AppColors.h"
#include "../globals/AppStyle.h"
#include "TitleTooltipWidget.h"
#include "TooltipWidget.h"
#include "../MetadataCache.h"

class VideoWidget
{
public:
	VideoWidget(Hosting& hosting);
	bool render();

private:
	// Dependency injection
	Hosting& _hosting;
	DX11& _dx11;
};