#pragma once

#include "../imgui/imgui.h"
#include <functional>

using namespace std;

class AbstractTooltipWidget
{
public:
	typedef function<void(void)> Action;

	static bool render(const char* title, AbstractTooltipWidget::Action callback = nullptr, bool forceShow = false);
};

