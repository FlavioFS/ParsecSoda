#include "AnimatedGamepadWidget.h"

void AnimatedGamepadWidget::updatePressed(const size_t pressId)
{
	m_lastPress[pressId] = m_currentTime;
}

const bool AnimatedGamepadWidget::isActive(const size_t pressId)
{
	if (difftime(m_currentTime, m_lastPress[pressId]) <= FADING_TIME) return true;
	return false;
}

void AnimatedGamepadWidget::render(XINPUT_GAMEPAD gamepad, float height, ImU32 activeColor)
{
	m_currentTime = time(NULL);

	float lDistance = 0, rDistance = 0;
	ImVec2 lStick = stickShortToFloat(gamepad.sThumbLX, gamepad.sThumbLY, lDistance);
	ImVec2 rStick = stickShortToFloat(gamepad.sThumbRX, gamepad.sThumbRY, rDistance);

	static float deadzone = 0.15f;

	bool isThumbR = (gamepad.wButtons & XUSB_GAMEPAD_RIGHT_THUMB) != 0;
	bool isThumbL = (gamepad.wButtons & XUSB_GAMEPAD_LEFT_THUMB) != 0;
	
	ImVec2 triggerLPos, triggerRPos;

	float radius = height / 2;
	renderAnalog(lStick, isThumbL, radius, lDistance <= deadzone, activeColor, PRESS_ID::LSTICK);
	renderHSpace();
	renderAnalog(rStick, isThumbR, radius, rDistance <= deadzone, activeColor, PRESS_ID::RSTICK);
	renderHSpace();
	renderDpad(gamepad.wButtons, height);
	renderHSpace();
	renderFaceButtons(gamepad.wButtons, height);
	renderHSpace();
	triggerLPos = ImGui::GetCursorScreenPos();
	renderTrigger(gamepad, false, height, activeColor, PRESS_ID::LTRIGGER);
	renderHSpace();
	triggerRPos = ImGui::GetCursorScreenPos();
	renderTrigger(gamepad, true, height, activeColor, PRESS_ID::RTRIGGER);
	renderGuideButton(gamepad, triggerLPos, triggerRPos, height);
}

void AnimatedGamepadWidget::renderAnalog(ImVec2 stick, bool isThumbPress, float radius, bool deadzoned, ImU32 activeColor, size_t pressId)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList = ImGui::GetWindowDrawList();

	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = ImVec2(p0.x + 2 * radius, p0.y + 2 * radius);
	ImVec2 center = (p0 + p1) * 0.5f;

	static ImU32 color;
	color = ANIMGAMEPAD_COL_BG;
	if (m_fading) {
		if (!deadzoned) updatePressed(pressId);
		color = isActive(pressId) ? ANIMGAMEPAD_ACTIVE_BG : ANIMGAMEPAD_INACTIVE_BG;
	}
	drawList->AddCircleFilled(center, radius, color);

	static ImU32 disabled = ImGui::GetColorU32(IM_COL32(25, 25, 25, 255));

	ImVec2 dotCenter = stick;
	float dotRatio = 0.4f;
	dotCenter.x = center.x + stick.x * radius * (1.0f - dotRatio);
	dotCenter.y = center.y - stick.y * radius * (1.0f - dotRatio);
	drawList->AddCircleFilled(dotCenter, dotRatio * radius, deadzoned ? disabled : activeColor);

	if (m_fading)
	{
		if (isThumbPress)
		{
			updatePressed(pressId + 1);
			drawList->AddCircle(center, radius, activeColor, 0, 0.25f * dotRatio * radius);
		}
		else if (isActive(pressId + 1))
		{
			drawList->AddCircle(center, radius, ANIMGAMEPAD_ACTIVE_BG, 0, 0.25f * dotRatio * radius);
		}
	}
	else if (isThumbPress)
	{
		drawList->AddCircle(center, radius, activeColor, 0, 0.25f * dotRatio * radius);
	}

	ImGui::Dummy(ImVec2(2*radius, 2*radius));
}

void AnimatedGamepadWidget::renderDpad(WORD wButtons, float height, ImU32 activeColor)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList = ImGui::GetWindowDrawList();

	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = ImVec2(p0.x + height, p0.y + height);
	ImVec2 center = (p0 + p1) * 0.5f;

	float dotRatio = 0.32f;
	float dotEdge = height * dotRatio;

	ImVec2 pUp    = ImVec2(center.x, center.y - height * 0.5f * (1.0f - dotRatio));
	ImVec2 pDown  = ImVec2(center.x, center.y + height * 0.5f * (1.0f - dotRatio));
	ImVec2 pLeft  = ImVec2(center.x - height * 0.5f * (1.0f - dotRatio), center.y);
	ImVec2 pRight = ImVec2(center.x + height * 0.5f * (1.0f - dotRatio), center.y);


	static ImU32 upColor, downColor, leftColor, rightColor;
	upColor = downColor = leftColor = rightColor = ANIMGAMEPAD_COL_BG;
	if (m_fading) {
		auto updateColor = [&](ImU32& color, XUSB_BUTTON button, PRESS_ID prid)
		{
			if ((wButtons & button)) updatePressed(prid);
			color = isActive(prid) ? ANIMGAMEPAD_ACTIVE_BG : ANIMGAMEPAD_INACTIVE_BG;
		};

		updateColor(upColor, XUSB_GAMEPAD_DPAD_UP, PRESS_ID::UP);
		updateColor(downColor, XUSB_GAMEPAD_DPAD_DOWN, PRESS_ID::DOWN);
		updateColor(leftColor, XUSB_GAMEPAD_DPAD_LEFT, PRESS_ID::LEFT);
		updateColor(rightColor, XUSB_GAMEPAD_DPAD_RIGHT, PRESS_ID::RIGHT);
	}

	renderSquare(drawList, pUp,    dotEdge, (wButtons & XUSB_GAMEPAD_DPAD_UP)    != 0 ? activeColor : upColor);
	renderSquare(drawList, pDown,  dotEdge, (wButtons & XUSB_GAMEPAD_DPAD_DOWN)  != 0 ? activeColor : downColor);
	renderSquare(drawList, pLeft,  dotEdge, (wButtons & XUSB_GAMEPAD_DPAD_LEFT)  != 0 ? activeColor : leftColor);
	renderSquare(drawList, pRight, dotEdge, (wButtons & XUSB_GAMEPAD_DPAD_RIGHT) != 0 ? activeColor : rightColor);
	
	ImGui::Dummy(ImVec2(height, height));
}

void AnimatedGamepadWidget::renderFaceButtons(WORD wButtons, float height)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList = ImGui::GetWindowDrawList();

	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = ImVec2(p0.x + height, p0.y + height);
	ImVec2 center = (p0 + p1) * 0.5f;

	float dotRatio = 0.32f;
	float dotRadius = height * dotRatio * 0.5f;

	ImVec2 pA = ImVec2(center.x, center.y + height * 0.5f * (1.0f - dotRatio));
	ImVec2 pB = ImVec2(center.x + height * 0.5f * (1.0f - dotRatio), center.y);
	ImVec2 pX = ImVec2(center.x - height * 0.5f * (1.0f - dotRatio), center.y);
	ImVec2 pY = ImVec2(center.x, center.y - height * 0.5f * (1.0f - dotRatio));

	static ImU32 aColor, bColor, xColor, yColor;
	aColor = bColor = xColor = yColor = ANIMGAMEPAD_COL_BG;
	if (m_fading) {
		auto updateColor = [&](ImU32& color, XUSB_BUTTON button, PRESS_ID prid)
		{
			if ((wButtons & button)) updatePressed(prid);
			color = isActive(prid) ? ANIMGAMEPAD_ACTIVE_BG : ANIMGAMEPAD_INACTIVE_BG;
		};

		updateColor(aColor, XUSB_GAMEPAD_A, PRESS_ID::A);
		updateColor(bColor, XUSB_GAMEPAD_B, PRESS_ID::B);
		updateColor(xColor, XUSB_GAMEPAD_X, PRESS_ID::X);
		updateColor(yColor, XUSB_GAMEPAD_Y, PRESS_ID::Y);
	}

	drawList->AddCircleFilled(pA, dotRadius, (wButtons & XUSB_GAMEPAD_A) != 0 ? ANIMGAMEPAD_COL_A : aColor);
	drawList->AddCircleFilled(pB, dotRadius, (wButtons & XUSB_GAMEPAD_B) != 0 ? ANIMGAMEPAD_COL_B : bColor);
	drawList->AddCircleFilled(pX, dotRadius, (wButtons & XUSB_GAMEPAD_X) != 0 ? ANIMGAMEPAD_COL_X : xColor);
	drawList->AddCircleFilled(pY, dotRadius, (wButtons & XUSB_GAMEPAD_Y) != 0 ? ANIMGAMEPAD_COL_Y : yColor);

	ImGui::Dummy(ImVec2(height, height));
}

void AnimatedGamepadWidget::renderTrigger(XINPUT_GAMEPAD gamepad, bool isRightTrigger, float height, ImU32 activeColor, size_t pressId)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList = ImGui::GetWindowDrawList();

	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = ImVec2(p0.x + height, p0.y + height);
	ImVec2 center = (p0 + p1) * 0.5f;

	float dotRatio = 0.32f;
	float dotRadius = height * dotRatio * 0.5f;

	ImVec2 pLTrigger  = ImVec2(center.x, center.y - height * 0.5f * (1.0f - dotRatio));

	float t = (float)(isRightTrigger ? gamepad.bRightTrigger : gamepad.bLeftTrigger) / 255;
	float minAngle = isRightTrigger ? -IM_PI : 0;
	float maxAngle = isRightTrigger ? 0 : -IM_PI;


	static ImU32 color;
	static XUSB_BUTTON button;


	auto updateColor = [&](size_t prid)
	{
		color = ANIMGAMEPAD_COL_BG;
		if (m_fading)
		{
			if ((gamepad.wButtons & button)) updatePressed(prid);
			color = isActive(prid) ? ANIMGAMEPAD_ACTIVE_BG : ANIMGAMEPAD_INACTIVE_BG;
		}
		color = (gamepad.wButtons & button) != 0 ? activeColor : color;
	};


	// ===============
	// Trigger
	// ===============
	color = ANIMGAMEPAD_COL_BG;
	if (m_fading) {
		if (t > 0) updatePressed(pressId);
		color = isActive(pressId) ? ANIMGAMEPAD_ACTIVE_BG : ANIMGAMEPAD_INACTIVE_BG;
	}

	drawList->PathArcTo(center, height * 0.5f * (1.0f - 0.5f * dotRatio), minAngle, maxAngle, 20);
	drawList->PathStroke(color, 0, dotRadius);
	drawList->PathArcTo(center, height * 0.5f * (1.0f - 0.5f * dotRatio), minAngle, minAngle * (1-t) + t * maxAngle, 20);
	drawList->PathStroke(activeColor, 0, dotRadius);
	

	// ===============
	// Shoulder
	// ===============
	button = isRightTrigger ? XUSB_GAMEPAD_RIGHT_SHOULDER : XUSB_GAMEPAD_LEFT_SHOULDER;
	updateColor(pressId + 1);

	float shoulderRadius = dotRadius * 0.6;
	renderSquare(drawList, center, 2.0f * shoulderRadius, color);
	drawList->AddCircleFilled(ImVec2(center.x + shoulderRadius, center.y), shoulderRadius, color);
	drawList->AddCircleFilled(ImVec2(center.x - shoulderRadius, center.y), shoulderRadius, color);


	// ===============
	// Start / Back
	// ===============
	ImVec2 pMiddle = ImVec2(center.x, center.y + height * 0.5f * (1.0f - dotRatio));
	ImVec2 pMiddle0, pMiddle1, pMiddle2;
	float triangleEdge = dotRadius * 1.0f;
	if (isRightTrigger)
	{
		button = XUSB_GAMEPAD_START;
		pMiddle0 = pMiddle + (ImVec2(-1.0f, -1.0f) * triangleEdge);
		pMiddle1 = pMiddle + (ImVec2(-1.0f, +1.0f) * triangleEdge);
		pMiddle2 = ImVec2(pMiddle.x + triangleEdge, pMiddle.y);
	}
	else
	{
		button = XUSB_GAMEPAD_BACK;
		pMiddle0 = pMiddle + (ImVec2(1.0f, -1.0f) * triangleEdge);
		pMiddle1 = pMiddle + (ImVec2(1.0f, +1.0f) * triangleEdge);
		pMiddle2 = ImVec2(pMiddle.x - triangleEdge, pMiddle.y);
	}

	updateColor(pressId + 2);
	drawList->AddTriangleFilled(pMiddle0, pMiddle1, pMiddle2, color);

	ImGui::Dummy(ImVec2(height, height));
}

void AnimatedGamepadWidget::renderGuideButton(XINPUT_GAMEPAD gamepad, ImVec2 triggerLPos, ImVec2 TriggerRPos, float height, ImU32 activeColor)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList = ImGui::GetWindowDrawList();

	float dotRatio = 0.32f;
	float dotRadius = height * dotRatio * 0.5f;

	ImVec2 p0 = triggerLPos;
	ImVec2 p1 = TriggerRPos + ImVec2(height, height);
	ImVec2 center = (p0 + p1) * 0.5f;
	ImVec2 target = ImVec2(center.x, center.y + height * 0.5f * (1.0f - dotRatio));

	static ImU32 color;
	static XUSB_BUTTON button;

	auto updateColor = [&](size_t prid)
	{
		color = ANIMGAMEPAD_COL_BG;
		if (m_fading)
		{
			if ((gamepad.wButtons & button)) updatePressed(prid);
			color = isActive(prid) ? ANIMGAMEPAD_ACTIVE_BG : ANIMGAMEPAD_INACTIVE_BG;
		}
		color = (gamepad.wButtons & button) != 0 ? activeColor : color;
	};

	// ===============
	// Guide
	// ===============
	button = XUSB_GAMEPAD_GUIDE;
	updateColor(PRESS_ID::GUIDE);
	drawList->AddCircleFilled(target, dotRadius, color);
}

void AnimatedGamepadWidget::tryResizeAnimations(vector<AnimatedGamepadWidget>& animations, const size_t size, const bool fading)
{
	if (animations.size() != size)
	{
		int diff = (int)size - (int)animations.size();

		if (diff > 0)
		{
			for (size_t i = 0; i < diff; i++)
			{
				animations.push_back(AnimatedGamepadWidget(fading));
			}
		}
		else
		{
			diff = -diff;
			for (size_t i = 0; i < diff; i++)
			{
				animations.pop_back();
			}
		}
	}
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