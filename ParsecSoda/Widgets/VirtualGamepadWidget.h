#pragma once

#include <cmath>
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../AGamepad.h"
#include "../globals/AppColors.h"
#include "../globals/AppIcons.h"
#include "IconButton.h"
#include <string>

#define IMVEC2_UP ImVec2(0.0f, 1.0f)
#define IMVEC2_DOWN ImVec2(0.0f, -1.0f)
#define IMVEC2_RIGHT ImVec2(1.0f, 0.0f)
#define IMVEC2_LEFT ImVec2(-1.0f, 0.0f)
#define VGAMEPAD_COL_BG COLOR_WIDGET_BG_U32
#define VGAMEPAD_COL_A  ImGui::GetColorU32(IM_COL32(89, 255, 89, 255))
#define VGAMEPAD_COL_B  ImGui::GetColorU32(IM_COL32(255, 102, 102, 255))
#define VGAMEPAD_COL_X  ImGui::GetColorU32(IM_COL32(115, 157, 255, 255))
#define VGAMEPAD_COL_Y  ImGui::GetColorU32(IM_COL32(255, 255, 115, 255))

class VirtualGamepadWidget
{
public:
	XINPUT_GAMEPAD& render(float height = 100.0f, ImU32 activeColor = COLOR_PRIMARY_U32);
	void renderAnalog(bool isLeftStick = true, float radius = 20.0f, ImU32 activeColor = COLOR_PRIMARY_U32);

private:
	enum class MouseButton
	{
		NONE = 0,
		LEFT = (1 << 0),
		RIGHT = (1 << 1),
		MIDDLE = (1 << 2),
		ALL = -1
	};

	void renderDpad(float edge = 40.0f, ImU32 activeColor = COLOR_PRIMARY_U32);
	void renderFaceButtons(float height = 40.0f);
	void renderTrigger(bool isLeftTrigger, float height = 40.0f, ImU32 activeColor = COLOR_PRIMARY_U32);
	void renderMenuAndBumper(bool isLeftSide, float height = 40.0f, ImU32 activeColor = COLOR_PRIMARY_U32);

	static void renderHSpace(float distance = 5.0f);
	static bool renderSquare(ImDrawList* drawlist, ImVec2 center, float edge, ImU32 color);
	static ImVec2 stickShortToFloat(SHORT lx, SHORT ly, float& distance);

	static bool isMouseDown(MouseButton mouseBtn = MouseButton::ALL);
	static bool checkSquarePress(ImVec2 upLeftCorner, ImVec2 downRightCorner, MouseButton mouseBtn = MouseButton::ALL);
	static bool checkCirclePress(ImVec2 center, float radius, MouseButton mouseBtn = MouseButton::ALL);

	static inline ImVec2 sum(const ImVec2& lhs, const ImVec2& rhs)
	{
		return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}
	static inline ImVec2 dif(const ImVec2& lhs, const ImVec2& rhs)
	{
		return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
	}
	static inline ImVec2 mul(const ImVec2& lhs, const float lambda)
	{
		return ImVec2(lhs.x * lambda, lhs.y * lambda);
	}
	static inline float sqModulus(const ImVec2& vec)
	{
		return vec.x * vec.x + vec.y * vec.y;
	}
	static inline ImVec2 lerp(const ImVec2& v1, const ImVec2& v2, const float t)
	{
		return sum(mul(v1, 1.0f - t), mul(v2, t));
	}
	static inline ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a)
	{
		return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a);
	}
	
	// ==============================================
	// Attributes
	// ==============================================
	XINPUT_GAMEPAD _gamepad;
};

