#pragma once

#include <Windows.h>
#include <cmath>
#include <string>
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

using namespace std;

class LoadingRingWidget
{
public:
	static void render(bool resetAnimation = false);

private:
	static inline ImVec2 sum(const ImVec2& lhs, const ImVec2& rhs)
	{
		return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}
	static inline ImVec2 mul(const ImVec2& lhs, const float alpha)
	{
		return ImVec2(lhs.x * alpha, lhs.y * alpha);
	}
};

