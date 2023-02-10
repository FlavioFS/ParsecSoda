#include "AnimatedGamepadWidget.h"

void AnimatedGamepadWidget::render(XINPUT_GAMEPAD gamepad, float height, ImU32 activeColor)
{
	float lDistance = 0, rDistance = 0;
	ImVec2 lStick = stickShortToFloat(gamepad.sThumbLX, gamepad.sThumbLY, lDistance);
	ImVec2 rStick = stickShortToFloat(gamepad.sThumbRX, gamepad.sThumbRY, rDistance);

	static float deadzone = 0.15f;

	bool isThumbR = (gamepad.wButtons & XUSB_GAMEPAD_RIGHT_THUMB) != 0;
	bool isThumbL = (gamepad.wButtons & XUSB_GAMEPAD_LEFT_THUMB) != 0;

	float radius = height / 2;
	renderAnalog(lStick, isThumbL, radius, lDistance <= deadzone, activeColor);
	renderHSpace();
	renderAnalog(rStick, isThumbR, radius, rDistance <= deadzone, activeColor);
	renderHSpace();
	renderDpad(gamepad.wButtons, height);
	renderHSpace();
	renderFaceButtons(gamepad.wButtons, height);
	renderHSpace();
	renderTrigger(gamepad, false, height, activeColor);
	renderHSpace();
	renderTrigger(gamepad, true, height, activeColor);
}

void AnimatedGamepadWidget::renderAnalog(ImVec2 stick, bool isThumbPress, float radius, bool deadzoned, ImU32 activeColor)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList = ImGui::GetWindowDrawList();

	static ImVec2 cursor;
	cursor = ImGui::GetCursorPos();

	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = ImVec2(p0.x + 2 * radius, p0.y + 2 * radius);
	ImVec2 center = (p0 + p1) * 0.5f;
	drawList->AddCircleFilled(center, radius, ANIMGAMEPAD_COL_BG);

	static ImU32 disabled = ImGui::GetColorU32(IM_COL32(25, 25, 25, 255));

	ImVec2 dotCenter = stick;
	float dotRatio = 0.4f;
	dotCenter.x = center.x + stick.x * radius * (1.0f - dotRatio);
	dotCenter.y = center.y - stick.y * radius * (1.0f - dotRatio);
	drawList->AddCircleFilled(dotCenter, dotRatio * radius, deadzoned ? disabled : activeColor);

	if (isThumbPress)
	{
		drawList->AddCircle(center, radius, activeColor, 0, 0.25f * dotRatio * radius);
	}

	ImGui::SetCursorPos(cursor);
	ImGui::Dummy(ImVec2(2*radius, 2*radius));
}

void AnimatedGamepadWidget::renderDpad(WORD wButtons, float height, ImU32 activeColor)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList = ImGui::GetWindowDrawList();
	static ImVec2 cursor;
	cursor = ImGui::GetCursorPos();

	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = ImVec2(p0.x + height, p0.y + height);
	ImVec2 center = (p0 + p1) * 0.5f;

	float dotRatio = 0.32f;
	float dotEdge = height * dotRatio;

	ImVec2 pUp    = ImVec2(center.x, center.y - height * 0.5f * (1.0f - dotRatio));
	ImVec2 pDown  = ImVec2(center.x, center.y + height * 0.5f * (1.0f - dotRatio));
	ImVec2 pLeft  = ImVec2(center.x - height * 0.5f * (1.0f - dotRatio), center.y);
	ImVec2 pRight = ImVec2(center.x + height * 0.5f * (1.0f - dotRatio), center.y);

	renderSquare(drawList, pUp,    dotEdge, (wButtons & XUSB_GAMEPAD_DPAD_UP)    != 0 ? activeColor : ANIMGAMEPAD_COL_BG);
	renderSquare(drawList, pDown,  dotEdge, (wButtons & XUSB_GAMEPAD_DPAD_DOWN)  != 0 ? activeColor : ANIMGAMEPAD_COL_BG);
	renderSquare(drawList, pLeft,  dotEdge, (wButtons & XUSB_GAMEPAD_DPAD_LEFT)  != 0 ? activeColor : ANIMGAMEPAD_COL_BG);
	renderSquare(drawList, pRight, dotEdge, (wButtons & XUSB_GAMEPAD_DPAD_RIGHT) != 0 ? activeColor : ANIMGAMEPAD_COL_BG);
	
	ImGui::SetCursorPos(cursor);
	ImGui::Dummy(ImVec2(height, height));
}

void AnimatedGamepadWidget::renderFaceButtons(WORD wButtons, float height)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList = ImGui::GetWindowDrawList();
	static ImVec2 cursor;
	cursor = ImGui::GetCursorPos();

	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = ImVec2(p0.x + height, p0.y + height);
	ImVec2 center = (p0 + p1) * 0.5f;

	float dotRatio = 0.32f;
	float dotRadius = height * dotRatio * 0.5f;

	ImVec2 pA = ImVec2(center.x, center.y + height * 0.5f * (1.0f - dotRatio));
	ImVec2 pB = ImVec2(center.x + height * 0.5f * (1.0f - dotRatio), center.y);
	ImVec2 pX = ImVec2(center.x - height * 0.5f * (1.0f - dotRatio), center.y);
	ImVec2 pY = ImVec2(center.x, center.y - height * 0.5f * (1.0f - dotRatio));

	drawList->AddCircleFilled(pA, dotRadius, (wButtons & XUSB_GAMEPAD_A) != 0 ? ANIMGAMEPAD_COL_A : ANIMGAMEPAD_COL_BG);
	drawList->AddCircleFilled(pB, dotRadius, (wButtons & XUSB_GAMEPAD_B) != 0 ? ANIMGAMEPAD_COL_B : ANIMGAMEPAD_COL_BG);
	drawList->AddCircleFilled(pX, dotRadius, (wButtons & XUSB_GAMEPAD_X) != 0 ? ANIMGAMEPAD_COL_X : ANIMGAMEPAD_COL_BG);
	drawList->AddCircleFilled(pY, dotRadius, (wButtons & XUSB_GAMEPAD_Y) != 0 ? ANIMGAMEPAD_COL_Y : ANIMGAMEPAD_COL_BG);

	ImGui::SetCursorPos(cursor);
	ImGui::Dummy(ImVec2(height, height));
}

void AnimatedGamepadWidget::renderTrigger(XINPUT_GAMEPAD gamepad, bool isRightTrigger, float height, ImU32 activeColor)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList = ImGui::GetWindowDrawList();
	static ImVec2 cursor;
	cursor = ImGui::GetCursorPos();

	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = ImVec2(p0.x + height, p0.y + height);
	ImVec2 center = (p0 + p1) * 0.5f;

	float dotRatio = 0.32f;
	float dotRadius = height * dotRatio * 0.5f;

	ImVec2 pLTrigger  = ImVec2(center.x, center.y - height * 0.5f * (1.0f - dotRatio));

	float t = (float)(isRightTrigger ? gamepad.bRightTrigger : gamepad.bLeftTrigger) / 255;
	float minAngle = isRightTrigger ? -IM_PI : 0;
	float maxAngle = isRightTrigger ? 0 : -IM_PI;
	drawList->PathArcTo(center, height * 0.5f * (1.0f - 0.5f * dotRatio), minAngle, maxAngle, 20);
	drawList->PathStroke(ANIMGAMEPAD_COL_BG, 0, dotRadius);
	drawList->PathArcTo(center, height * 0.5f * (1.0f - 0.5f * dotRatio), minAngle, minAngle * (1-t) + t * maxAngle, 20);
	drawList->PathStroke(activeColor, 0, dotRadius);
	
	ImU32 color = ANIMGAMEPAD_COL_BG;
	if (isRightTrigger)
		color = (gamepad.wButtons & XUSB_GAMEPAD_RIGHT_SHOULDER) != 0 ? activeColor : ANIMGAMEPAD_COL_BG;
	else
		color = (gamepad.wButtons & XUSB_GAMEPAD_LEFT_SHOULDER) != 0 ? activeColor : ANIMGAMEPAD_COL_BG;

	float shoulderRadius = dotRadius * 0.6;
	renderSquare(drawList, center, 2.0f * shoulderRadius, color);
	drawList->AddCircleFilled(ImVec2(center.x + shoulderRadius, center.y), shoulderRadius, color);
	drawList->AddCircleFilled(ImVec2(center.x - shoulderRadius, center.y), shoulderRadius, color);


	ImVec2 pMiddle = ImVec2(center.x, center.y + height * 0.5f * (1.0f - dotRatio));
	ImVec2 pMiddle0, pMiddle1, pMiddle2;

	if (isRightTrigger)
	{
		pMiddle0 = pMiddle + (ImVec2(-1.0f, -1.0f) * shoulderRadius);
		pMiddle1 = pMiddle + (ImVec2(-1.0f, +1.0f) * shoulderRadius);
		pMiddle2 = ImVec2(pMiddle.x + shoulderRadius, pMiddle.y);
		color = (gamepad.wButtons & XUSB_GAMEPAD_START) != 0 ? activeColor : ANIMGAMEPAD_COL_BG;
		drawList->AddTriangleFilled(pMiddle0, pMiddle1, pMiddle2, color);
	}
	else
	{
		pMiddle0 = pMiddle + (ImVec2(1.0f, -1.0f) * shoulderRadius);
		pMiddle1 = pMiddle + (ImVec2(1.0f, +1.0f) * shoulderRadius);
		pMiddle2 = ImVec2(pMiddle.x - shoulderRadius, pMiddle.y);
		color = (gamepad.wButtons & XUSB_GAMEPAD_BACK) != 0 ? activeColor : ANIMGAMEPAD_COL_BG;
		drawList->AddTriangleFilled(pMiddle0, pMiddle1, pMiddle2, color);

		//ImU32 pGuideColor = ANIMGAMEPAD_COL_BG;
		//if (useFading) {
		//	if ((gamepad.wButtons & XUSB_GAMEPAD_GUIDE)) updatePressed(pressId + 3);
		//	pGuideColor = isActive(pressId + 3) ? ANIMGAMEPAD_ACTIVE_BG : ANIMGAMEPAD_INACTIVE_BG;
		//}
		//ImU32 guideColor = (gamepad.wButtons & XUSB_GAMEPAD_GUIDE) != 0 ? activeColor : pGuideColor;
		//drawList->AddCircleFilled(ImVec2(pMiddle1.x + 2.5f + (pMiddle1.y - pMiddle0.y) / 1.5, pMiddle2.y), (pMiddle1.y - pMiddle0.y) / 2, guideColor);
	}

	ImGui::SetCursorPos(cursor);
	ImGui::Dummy(ImVec2(height, height));
}

void AnimatedGamepadWidget::renderHSpace(float distance)
{
	ImGui::SameLine();
	ImGui::Dummy(ImVec2(distance, 0));
	ImGui::SameLine();
}

void AnimatedGamepadWidget::renderSquare(ImDrawList* drawlist, ImVec2 center, float edge, ImU32 color)
{
	ImVec2 p0 = center + (ImVec2(-0.5f, 0.5f) * edge);
	ImVec2 p1 = center + (ImVec2(0.5f, -0.5f) * edge);
	drawlist->AddRectFilled(p0, p1, color);
}

ImVec2 AnimatedGamepadWidget::stickShortToFloat(SHORT lx, SHORT ly, float& distance)
{
	static float shortMax = 32768;
	ImVec2 stick = ImVec2(
		(float)lx / shortMax,
		(float)ly / shortMax
	);
	stick.x = min(max(stick.x, -1.0f), 1.0f);
	stick.y = min(max(stick.y, -1.0f), 1.0f);

	distance = sqrtf(stick.x * stick.x + stick.y * stick.y);
	if (distance > 0.01f)
	{
		if (abs(lx) > abs(ly))
		{
			if (abs(stick.x) > 0.01f)
			{
				float yy = stick.y / abs(stick.x);
				float xx = 1.0f;
				float maxDiagonal = sqrtf(xx * xx + yy * yy);
				if (maxDiagonal > 0.01f)
				{
					stick.x /= maxDiagonal;
					stick.y /= maxDiagonal;
				}
			}
		}
		else
		{
			if (abs(stick.y) > 0.01f)
			{
				float xx = stick.x / abs(stick.y);
				float yy = 1.0f;
				float maxDiagonal = sqrtf(xx * xx + yy * yy);
				if (maxDiagonal > 0.01f)
				{
					stick.x /= maxDiagonal;
					stick.y /= maxDiagonal;
				}
			}
		}
	}

	return stick;
}

void AnimatedGamepadWidget::drawTrigger()
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList = ImGui::GetWindowDrawList();
	static ImVec2 cursor, a, b, c, d, uva, uvb, uvc, uvd, triggerCenter, triggerSize;
	//static float angle = IM_PI / 12.0f;
	cursor = ImGui::GetCursorPos();

	//ImGui::Image(AppIcons::gamepad_TriggerBG, ImVec2(32.0f, 46.0f));
	ImGui::Dummy(ImVec2(150, 100));

	ImGui::SetCursorPos(cursor);

	//ImVec2 gradient_size = ImVec2(ImGui::CalcItemWidth(), ImGui::GetFrameHeight());

	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = ImVec2(p0.x + 100, p0.y + 100);
	ImVec2 center = (p0 + p1) * 0.5f;
	ImU32 col_a = ImGui::GetColorU32(IM_COL32(54, 54, 54, 255));
	ImU32 col_b = ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));
	static float angle_min_left = IM_PI / 4.0f;
	static float angle_max_left = -IM_PI * 5.0f / 4.0f;
	drawList->PathArcTo(center, 15.0f, angle_min_left, angle_max_left, 20);
	drawList->PathStroke(col_a, 0, 15.0f);
}

void AnimatedGamepadWidget::RenderImageRotated(ImTextureID tex_id, ImVec2 center, ImVec2 size, float angle)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list = ImGui::GetWindowDrawList();

	float cos_a = cosf(angle);
	float sin_a = sinf(angle);
	ImVec2 pos[4] =
	{
		center + ImRotate(ImVec2(-size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a),
		center + ImRotate(ImVec2(+size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a),
		center + ImRotate(ImVec2(+size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a),
		center + ImRotate(ImVec2(-size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a)
	};
	ImVec2 uvs[4] =
	{
		ImVec2(0.0f, 0.0f),
		ImVec2(1.0f, 0.0f),
		ImVec2(1.0f, 1.0f),
		ImVec2(0.0f, 1.0f)
	};

	draw_list->AddImageQuad(tex_id, pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], IM_COL32_WHITE);
}