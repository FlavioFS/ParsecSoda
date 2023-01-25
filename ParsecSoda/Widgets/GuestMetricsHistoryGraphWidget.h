#pragma once

#include "../imgui/imgui.h"
#include "../imgui/implot.h"
#include "../GuestData.h"
#include "../Guest.h"
#include "../CircularMetrics.h"
#include "../globals/AppColors.h"
#include "../globals/AppStyle.h"
#include <cmath>

class GuestMetricsHistoryGraphWidget
{
public:
	static void render(GuestData guest, CircularMetrics& metricsHistory);
};