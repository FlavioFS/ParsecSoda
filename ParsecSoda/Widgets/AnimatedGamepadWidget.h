#pragma once

#include <cmath>
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../Gamepad.h"
#include "../globals/AppColors.h"

class AnimatedGamepadWidget
{
public:
	static void render(XINPUT_GAMEPAD gamepad, float stickRadius = 20.0f);
	static void renderAnalog(ImVec2 stick, float radius = 20.0f, ImU32 dotColor = ImGui::GetColorU32(IM_COL32(0, 120, 204, 255)));

private:
	static ImVec2 stickShortToFloat(SHORT lx, SHORT ly, float& distance);

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

