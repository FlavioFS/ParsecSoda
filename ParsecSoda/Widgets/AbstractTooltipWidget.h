#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppColors.h"
#include "../globals/AppStyle.h"
#include <functional>

using namespace std;

class AbstractTooltipWidget
{
public:
	typedef function<void(void)> Action;

	static bool render(const char* title, const char* description, AbstractTooltipWidget::Action callback = nullptr, bool forceShow = false);
};

