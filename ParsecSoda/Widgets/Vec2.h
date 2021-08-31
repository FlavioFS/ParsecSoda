#pragma once

#include "../imgui/imgui.h"

class Vec2
{
public:
	static inline ImVec2 sum(const ImVec2& lhs, const ImVec2& rhs)
	{
		return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}
	static inline ImVec2 mul(const ImVec2& lhs, const float lambda)
	{
		return ImVec2(lhs.x * lambda, lhs.y * lambda);
	}
};

