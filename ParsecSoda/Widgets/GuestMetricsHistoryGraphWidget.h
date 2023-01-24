#pragma once

#include "../imgui/imgui.h"
#include "../imgui/implot.h"
#include "../GuestData.h"
#include "../Guest.h"
#include "../CircularList.h"
#include "../globals/AppColors.h"

class GuestMetricsHistoryGraphWidget
{
public:
	static void render(GuestData guest, CircularList<GuestMetrics>& metricsHistory);
};