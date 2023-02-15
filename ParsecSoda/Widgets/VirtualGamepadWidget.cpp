#include "VirtualGamepadWidget.h"

XINPUT_GAMEPAD& VirtualGamepadWidget::render(float height, ImU32 activeColor)
{
	float radius = height / 2;
	renderAnalog(true, radius, activeColor);
	renderHSpace();
	renderAnalog(false, radius, activeColor);
	renderHSpace();
	renderDpad(height);
	renderHSpace();
	renderFaceButtons(height);
	renderHSpace();
	renderTrigger(true, height, activeColor);
	renderHSpace();
	renderTrigger(false, height, activeColor);
	renderHSpace();
	renderMenuAndBumper(true, height, activeColor);
	renderHSpace();
	renderMenuAndBumper(false, height, activeColor);

	return _gamepad;
}

void VirtualGamepadWidget::renderAnalog(bool isLeftStick, float radius, ImU32 activeColor)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList = ImGui::GetWindowDrawList();

	static ImVec2 cursor;
	cursor = ImGui::GetCursorPos();


	float lDistance = 0;
	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = p0 + Vector2::one * 2 * radius;
	ImVec2 center = (p0 + p1) * 0.5f;
	drawList->AddCircleFilled(center, radius, VGAMEPAD_COL_BG);

	static ImU32 disabled = ImGui::GetColorU32(IM_COL32(25, 25, 25, 255));

	if (isMouseDown(MouseButton::LEFT))
	{
		bool debug = true;
	}

	ImVec2 stick;
	bool isStickTilt = checkSquarePress(p0, p1, MouseButton::LEFT);	
	if (isStickTilt)
	{
		ImVec2 mouse = ImGui::GetMousePos();
		stick = (mouse - center) * (1.0f / radius);
		stick.x = max(min(stick.x, 1.0f), -1.0f);
		stick.y = max(min(stick.y, 1.0f), -1.0f);
		stick.y = -stick.y;
	}
	else
	{
		stick.x = 0;
		stick.y = 0;
	}
	SHORT xAxis = stick.x >= 0 ? SHRT_MAX * stick.x : SHRT_MIN * -stick.x;
	SHORT yAxis = stick.y >= 0 ? SHRT_MAX * stick.y : SHRT_MIN * -stick.y;

	if (isLeftStick)
	{
		_gamepad.sThumbLX = xAxis;
		_gamepad.sThumbLY = yAxis;
	}
	else
	{
		_gamepad.sThumbRX = xAxis;
		_gamepad.sThumbRY = yAxis;
	}

	ImVec2 dotCenter = stick;
	float dotRatio = 0.4f;
	dotCenter = center + ImVec2(stick.x, -stick.y) * radius * (1.0f - dotRatio);

	drawList->AddCircleFilled(dotCenter, dotRatio * radius, isStickTilt ? activeColor : disabled);

	bool isThumbPress = checkCirclePress(center, radius, MouseButton::RIGHT);
	Bitwise::setValue(_gamepad.wButtons, isLeftStick ? XUSB_GAMEPAD_LEFT_THUMB : XUSB_GAMEPAD_RIGHT_THUMB, isThumbPress);

	if (isThumbPress)
	{
		drawList->AddCircle(center, radius, activeColor, 0, 0.25f * dotRatio * radius);
	}

	ImGui::SetCursorPos(cursor);
	ImGui::Dummy(Vector2::one * 2 * radius);
}

void VirtualGamepadWidget::renderDpad(float height, ImU32 activeColor)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList = ImGui::GetWindowDrawList();
	static ImVec2 cursor;
	cursor = ImGui::GetCursorPos();

	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = p0 + Vector2::one * height;
	ImVec2 center = (p0 + p1) * 0.5f;

	float dotRatio = 0.32f;
	float dotEdge = height * dotRatio;

	float pDistance = height * 0.5f * (1.0f - dotRatio);
	ImVec2 pUp    = center + Vector2::down   * pDistance;
	ImVec2 pDown  = center + Vector2::up     * pDistance;
	ImVec2 pLeft  = center + Vector2::left  * pDistance;
	ImVec2 pRight = center + Vector2::right   * pDistance;

	static bool up, down, left, right;

	up    = renderSquare(drawList, pUp,    dotEdge, (_gamepad.wButtons & XUSB_GAMEPAD_DPAD_UP)    != 0 ? activeColor : VGAMEPAD_COL_BG);
	down  = renderSquare(drawList, pDown,  dotEdge, (_gamepad.wButtons & XUSB_GAMEPAD_DPAD_DOWN) != 0 ? activeColor : VGAMEPAD_COL_BG);
	left  = renderSquare(drawList, pLeft,  dotEdge, (_gamepad.wButtons & XUSB_GAMEPAD_DPAD_LEFT) != 0 ? activeColor : VGAMEPAD_COL_BG);
	right = renderSquare(drawList, pRight, dotEdge, (_gamepad.wButtons & XUSB_GAMEPAD_DPAD_RIGHT) != 0 ? activeColor : VGAMEPAD_COL_BG);

	Bitwise::setValue(_gamepad.wButtons, XUSB_GAMEPAD_DPAD_UP,    up);
	Bitwise::setValue(_gamepad.wButtons, XUSB_GAMEPAD_DPAD_DOWN,  down);
	Bitwise::setValue(_gamepad.wButtons, XUSB_GAMEPAD_DPAD_LEFT,  left);
	Bitwise::setValue(_gamepad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, right);
	
	ImGui::SetCursorPos(cursor);
	ImGui::Dummy(ImVec2(height, height));
}

void VirtualGamepadWidget::renderFaceButtons(float height)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList = ImGui::GetWindowDrawList();
	static ImVec2 cursor;
	cursor = ImGui::GetCursorPos();

	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = p0 + ImVec2(1, 1) * height;
	ImVec2 center = (p0 + p1) * 0.5f;

	float dotRatio = 0.32f;
	float dotRadius = height * dotRatio * 0.5f;

	float pDistance = height * 0.5f * (1.0f - dotRatio);
	ImVec2 pA = center + Vector2::up    * pDistance;
	ImVec2 pB = center + Vector2::right * pDistance;
	ImVec2 pX = center + Vector2::left  * pDistance;
	ImVec2 pY = center + Vector2::down  * pDistance;

	drawList->AddCircleFilled(pA, dotRadius, (_gamepad.wButtons & XUSB_GAMEPAD_A) != 0 ? VGAMEPAD_COL_A : VGAMEPAD_COL_BG);
	drawList->AddCircleFilled(pB, dotRadius, (_gamepad.wButtons & XUSB_GAMEPAD_B) != 0 ? VGAMEPAD_COL_B : VGAMEPAD_COL_BG);
	drawList->AddCircleFilled(pX, dotRadius, (_gamepad.wButtons & XUSB_GAMEPAD_X) != 0 ? VGAMEPAD_COL_X : VGAMEPAD_COL_BG);
	drawList->AddCircleFilled(pY, dotRadius, (_gamepad.wButtons & XUSB_GAMEPAD_Y) != 0 ? VGAMEPAD_COL_Y : VGAMEPAD_COL_BG);

	static bool A, B, X, Y;
	A = checkCirclePress(pA, dotRadius);
	B = checkCirclePress(pB, dotRadius);
	X = checkCirclePress(pX, dotRadius);
	Y = checkCirclePress(pY, dotRadius);

	Bitwise::setValue(_gamepad.wButtons, XUSB_GAMEPAD_A, A);
	Bitwise::setValue(_gamepad.wButtons, XUSB_GAMEPAD_B, B);
	Bitwise::setValue(_gamepad.wButtons, XUSB_GAMEPAD_X, X);
	Bitwise::setValue(_gamepad.wButtons, XUSB_GAMEPAD_Y, Y);

	ImGui::SetCursorPos(cursor);
	ImGui::Dummy(ImVec2(height, height));
}

void VirtualGamepadWidget::renderTrigger(bool isLeftTrigger, float height, ImU32 activeColor)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList = ImGui::GetWindowDrawList();
	static ImVec2 cursor;
	cursor = ImGui::GetCursorPos();

	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = p0 + ImVec2(1.0f/3.0f, 1) * height;
	ImVec2 center = (p0 + p1) * 0.5f;

	float t = 0;
	BYTE trigger = 0;
	if (checkSquarePress(p0, p1))
	{
		ImVec2 mouse = ImGui::GetMousePos();
		t = (mouse.y - p1.y) / (p0.y - p1.y);
		trigger = (BYTE)(t * (BYTE)-1);
	}

	if (isLeftTrigger) _gamepad.bLeftTrigger = trigger;
	else _gamepad.bRightTrigger = trigger;

	ImVec2 pFill = ImVec2(
		p0.x,
		p0.y * t + p1.y * (1-t)
	);

	drawList->AddRectFilled(p0, p1, VGAMEPAD_COL_BG);
	drawList->AddRectFilled(pFill, p1, activeColor);
	
	ImGui::SetCursorPos(cursor);
	ImGui::Dummy(ImVec2(height/3.0f, height));
}

void VirtualGamepadWidget::renderMenuAndBumper(bool isLeftSide, float height, ImU32 activeColor)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList = ImGui::GetWindowDrawList();
	static ImVec2 cursor;
	cursor = ImGui::GetCursorPos();

	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = p0 + Vector2::one * height;
	ImVec2 center = (p0 + p1) * 0.5f;

	float offset = height / 3.0f;
	float radius = offset / 2.0f;

	// ===============================================
	// Bumper
	// ===============================================
	ImVec2 pBumper  = center + Vector2::down * offset;
	ImVec2 pBumperL = pBumper + Vector2::left * radius;
	ImVec2 pBumperR = pBumper + Vector2::right * radius;
	ImVec2 pBumper0 = pBumperL + Vector2::down * radius;
	ImVec2 pBumper1 = pBumperR + Vector2::up * radius;
	ImVec2 pBumperCheck0 = pBumper0 + Vector2::left * radius;
	ImVec2 pBumperCheck1 = pBumper1 + Vector2::right * radius;
	
	bool isBumperPressed = checkSquarePress(pBumperCheck0, pBumperCheck1);
	ImU32 bumperColor = isBumperPressed ? activeColor : VGAMEPAD_COL_BG;
	drawList->AddRectFilled(pBumper0, pBumper1, bumperColor);
	drawList->AddCircleFilled(pBumperL, radius, bumperColor);
	drawList->AddCircleFilled(pBumperR, radius, bumperColor);


	// ===============================================
	// Menu
	// ===============================================
	ImVec2 pMenu, pMenu0, pMenu1, pMenu2, pMenuCheck0, pMenuCheck1;
	pMenu = center + Vector2::up * offset;
	if (isLeftSide)
	{
		pMenu0 = pMenu + (Vector2::right + Vector2::down) * radius;
		pMenu1 = pMenu + (Vector2::right + Vector2::up)   * radius;
		pMenu2 = pMenu + Vector2::left * 2 * radius;
		pMenuCheck0 = ImVec2(pMenu2.x, pMenu0.y);
		pMenuCheck1 = pMenu1;
	}
	else
	{
		pMenu0 = pMenu + (Vector2::left + Vector2::down) * radius;
		pMenu1 = pMenu + (Vector2::left + Vector2::up) * radius;
		pMenu2 = pMenu + Vector2::right * 2 * radius;
		pMenuCheck0 = pMenu0;
		pMenuCheck1 = ImVec2(pMenu2.x, pMenu1.y);
	}
	
	bool isMenuPressed = checkSquarePress(pMenuCheck0, pMenuCheck1);
	ImU32 menuColor = isMenuPressed ? activeColor : VGAMEPAD_COL_BG;
	drawList->AddTriangleFilled(pMenu0, pMenu1, pMenu2, menuColor);
	

	// ===============================================
	// Update Gamepad
	// ===============================================
	Bitwise::setValue(_gamepad.wButtons, isLeftSide ? XUSB_GAMEPAD_LEFT_SHOULDER : XUSB_GAMEPAD_RIGHT_SHOULDER, isBumperPressed);
	Bitwise::setValue(_gamepad.wButtons, isLeftSide ? XUSB_GAMEPAD_BACK : XUSB_GAMEPAD_START, isMenuPressed);

	ImGui::SetCursorPos(cursor);
	ImGui::Dummy(ImVec2(2.0f / 3.0f, 1.0f) * height);
}

void VirtualGamepadWidget::renderHSpace(float distance)
{
	ImGui::SameLine();
	ImGui::Dummy(Vector2::right * distance);
	ImGui::SameLine();
}

bool VirtualGamepadWidget::renderSquare(ImDrawList* drawlist, ImVec2 center, float edge, ImU32 color)
{
	ImVec2 p0 = center - Vector2::one * (0.5f * edge);
	ImVec2 p1 = center + Vector2::one * (0.5f * edge);
	drawlist->AddRectFilled(p0, p1, color);
	return checkSquarePress(p0, p1);
}

ImVec2 VirtualGamepadWidget::stickShortToFloat(SHORT lx, SHORT ly, float& distance)
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
					stick = stick / maxDiagonal;
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
					stick = stick / maxDiagonal;
				}
			}
		}
	}

	return stick;
}

bool VirtualGamepadWidget::isMouseDown(MouseButton mouseBtn)
{
	bool result = false;

	if (((int)mouseBtn & (int)MouseButton::ALL) != (int)MouseButton::NONE)
	{
		if (((int)mouseBtn & (int)MouseButton::LEFT) != (int)MouseButton::NONE)
			result = result || ImGui::IsMouseDown(ImGuiMouseButton_Left);

		if (((int)mouseBtn & (int)MouseButton::RIGHT) != (int)MouseButton::NONE)
			result = result || ImGui::IsMouseDown(ImGuiMouseButton_Right);

		if (((int)mouseBtn & (int)MouseButton::MIDDLE) != (int)MouseButton::NONE)
			result = result || ImGui::IsMouseDown(ImGuiMouseButton_Middle);
	}

	return result;
}

bool VirtualGamepadWidget::checkSquarePress(ImVec2 upLeftCorner, ImVec2 downRightCorner, MouseButton mouseBtn)
{
	static ImVec2 mouse;
	mouse = ImGui::GetMousePos();
	return
		isMouseDown(mouseBtn) &&
		mouse.x >= upLeftCorner.x && mouse.x <= downRightCorner.x &&
		mouse.y >= upLeftCorner.y && mouse.y <= downRightCorner.y
		;
}

bool VirtualGamepadWidget::checkCirclePress(ImVec2 center, float radius, MouseButton mouseBtn)
{
	static ImVec2 mouse, distance;
	mouse = ImGui::GetMousePos();
	distance = mouse - center;
	return 
		isMouseDown(mouseBtn) &&
		Vector2::sqMod(distance) <= radius * radius
		;
}