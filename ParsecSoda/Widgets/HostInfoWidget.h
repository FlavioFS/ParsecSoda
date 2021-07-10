#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "../Hosting.h"
#include "TitleTooltipWidget.h"

class HostInfoWidget
{
public:
	HostInfoWidget(Hosting& hosting);
	void render();

private:
	Hosting& _hosting;
};

