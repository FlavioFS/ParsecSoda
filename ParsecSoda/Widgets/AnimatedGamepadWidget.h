#pragma once

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/ImVecExtensions.h"
#include "../AGamepad.h"
#include "../globals/AppColors.h"
#include <cmath>
#include <functional>
#include <vector>

#define IMVEC2_UP ImVec2(0.0f, 1.0f)
#define IMVEC2_DOWN ImVec2(0.0f, -1.0f)
#define IMVEC2_RIGHT ImVec2(1.0f, 0.0f)
#define IMVEC2_LEFT ImVec2(-1.0f, 0.0f)
#define ANIMGAMEPAD_COL_BG COLOR_WIDGET_BG_U32
#define ANIMGAMEPAD_COL_A  ImGui::GetColorU32(IM_COL32(89, 255, 89, 255))
#define ANIMGAMEPAD_COL_B  ImGui::GetColorU32(IM_COL32(255, 102, 102, 255))
#define ANIMGAMEPAD_COL_X  ImGui::GetColorU32(IM_COL32(115, 157, 255, 255))
#define ANIMGAMEPAD_COL_Y  ImGui::GetColorU32(IM_COL32(255, 255, 115, 255))
#define ANIMGAMEPAD_ACTIVE_BG  ImGui::GetColorU32(IM_COL32(55, 55, 55, 255))
#define ANIMGAMEPAD_INACTIVE_BG  ImGui::GetColorU32(IM_COL32(20, 20, 20, 255))

class AnimatedGamepadWidget
{
public:
	static const uint32_t FADING_TIME = 3;

	AnimatedGamepadWidget(bool fading = true) {
		m_fading = fading;
	}
	void render(XINPUT_GAMEPAD gamepad, float height = 40.0f, ImU32 activeColor = COLOR_PRIMARY_U32);
	void renderAnalog(ImVec2 stick, bool isThumbPress, float radius = 20.0f, bool deadzoned = false, ImU32 activeColor = COLOR_PRIMARY_U32, size_t pressId = 0);
	void renderDpad(WORD wButtons, float edge = 40.0f, ImU32 activeColor = COLOR_PRIMARY_U32);
	void renderFaceButtons(WORD wButtons, float height = 40.0f);
	void renderTrigger(XINPUT_GAMEPAD gamepad, bool isRightTrigger, float height = 40.0f, ImU32 activeColor = COLOR_PRIMARY_U32, size_t pressId = 0);
	void renderGuideButton(XINPUT_GAMEPAD gamepad, ImVec2 triggerLPos, ImVec2 TriggerRPos, float height = 40.0f, ImU32 activeColor = COLOR_PRIMARY_U32);

	static void tryResizeAnimations(vector<AnimatedGamepadWidget>& animations, const size_t size, const bool fading = true);

private:
	static enum PRESS_ID
	{
		LSTICK, LTHUMB, RSTICK, RTHUMB,
		UP, DOWN, LEFT, RIGHT,
		A, B, X, Y,
		LTRIGGER, LSHOULDER, BACK, GUIDE,
		RTRIGGER, RSHOULDER, START
	};

	void renderHSpace(float distance = 5.0f);
	void renderSquare(ImDrawList* drawlist, ImVec2 center, float edge, ImU32 color);
	ImVec2 stickShortToFloat(SHORT lx, SHORT ly, float& distance);

	void drawTrigger();

	static void RenderImageRotated(ImTextureID tex_id, ImVec2 center, ImVec2 size, float angle);

	void updatePressed(const size_t pressId);
	const bool isActive(const size_t pressId);

	bool m_fading = false;
	time_t m_lastPress[20] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	time_t m_currentTime = 0;
};

