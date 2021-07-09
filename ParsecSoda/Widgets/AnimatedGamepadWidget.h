#pragma once

#include <cmath>
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../Gamepad.h"

class AnimatedGamepadWidget
{
public:
	static void render(string widgetID, Gamepad& gamepad);

private:
	static void drawTrigger();

	static inline ImVec2 sum(const ImVec2& lhs, const ImVec2& rhs)
	{
		return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}
	static inline ImVec2 mul(const ImVec2& lhs, const float alpha)
	{
		return ImVec2(lhs.x * alpha, lhs.y * alpha);
	}
	static inline ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a)
	{
		return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a);
	}

	static void RenderImageRotated(ImTextureID tex_id, ImVec2 center, ImVec2 size, float angle);
};

