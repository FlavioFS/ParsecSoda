#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "../ThumbnailList.h"
#include "../Clock.h"
#include "../Debouncer.h"
#include "../ParsecSession.h"
#include "IconButton.h"
#include "ProgressCircularWidget.h"
#include "TitleTooltipWidget.h"

class ThumbnailsWidget
{
public:
	static bool render(ParsecSession& session);
};

