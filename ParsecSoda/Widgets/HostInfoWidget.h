#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "../Hosting.h"
#include "../Clock.h"
#include "../Stopwatch.h"
#include "../Debouncer.h"
#include "TitleTooltipWidget.h"
#include "ProgressCircularWidget.h"


class HostInfoWidget
{
public:
	HostInfoWidget(Hosting& hosting);
	void render();

private:
	Hosting& _hosting;
	uint32_t _loginTime;
};

