#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"

#include "IconButton.h"

#include <Xinput.h>
#include "ViGEm/Client.h"
#include "IntRangeWidget.h"

#include <vector>
#include <concrt.h>

#include <cstdint>
#include <iostream>
#include <roapi.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Gaming.Input.h>

using namespace std;
using namespace winrt;
using namespace winrt::Windows::Gaming;

class HostGamepadWidget
{
public:
	static bool fetchSystemGamepads();
	static bool render();

private:
	static vector<Input::Gamepad> _systemGamepads;
};

