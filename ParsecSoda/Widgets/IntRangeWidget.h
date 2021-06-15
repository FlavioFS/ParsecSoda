#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppStyle.h"
#include <string>

using namespace std;

class IntRangeWidget
{
public:
	static bool render(const char* id, int& value, int minimum = 0, int maximum = 100, float speed = 0.05f);
};

