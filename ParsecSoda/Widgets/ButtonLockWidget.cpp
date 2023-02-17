#include "ButtonLockWidget.h"


bool ButtonLockWidget::render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 5));

	AppStyle::pushTitle();
	ImGui::SetNextWindowSize(ImVec2(532, 490));
	ImGui::Begin("Button Lock", (bool*)0, ImGuiWindowFlags_NoResize);
	AppStyle::pop();
	AppStyle::pushInput();

	if (SwitchWidget::render(
		MetadataCache::preferences.buttonLock.isEnabled,
		"Button Lock Switch", "",
		"Button Lock [ON]", "Mark buttons below to block their inputs.",
		"Button Lock [OFF]", "All button inputs allowed."
	))
	{
		MetadataCache::savePreferences();
	}

	ImGui::SameLine();

	if (BadgeButtonWidget::render(AppIcons::unlock, "Unlock all", "Press to clear all buttons.", "Unlock all buttons", ImVec2(25, 25), AppColors::primary))
	{
		MetadataCache::preferences.buttonLock.unlockAll();
		MetadataCache::savePreferences();
	}

	ImGui::SameLine();

	if (BadgeButtonWidget::render(AppIcons::lock, "Lock all", "Press to lock all buttons.", "Lock all buttons", ImVec2(25, 25), AppColors::red))
	{
		MetadataCache::preferences.buttonLock.lockAll();
		MetadataCache::savePreferences();
	}

	ImGui::Separator();
	AppStyle::pop();

	renderGamepad(AppIcons::gamepadBg.height);

	ImGui::End();
	ImGui::PopStyleVar();
	return false;
}

void ButtonLockWidget::renderGamepad(const float& height)
{
	static bool buttonState = false;
	static ImVec2 cursor, size, gamepadSize, center, groupCenter;
	static ImVec4 tint;
	static float ratio, groupOffset;
	ratio = (height / AppIcons::gamepadBg.height);

	cursor = ImGui::GetCursorPos();
	gamepadSize = ImVec2(AppIcons::gamepadBg.width, AppIcons::gamepadBg.height) * ratio;
	center = cursor + gamepadSize * 0.5f;
	ImGui::Image(AppIcons::gamepadBg.texture, gamepadSize);


	// ===================================
	// Shoulders
	// ===================================
	size = ImVec2(AppIcons::gamepadBumper.width, AppIcons::gamepadBumper.height) * ratio;
	ImGui::SetCursorPos(center - size * 0.5f + ImVec2(-0.224f, -0.2f) * gamepadSize);
	renderBitmaskTextureButton(
		XUSB_GAMEPAD_LEFT_SHOULDER, "Left Bumper", AppIcons::gamepadBumper.texture, size,
		"Left Bumper [Locked]", "Left Bumper [Unlocked]",
		"Press to UNLOCK Left Bumper.", "Press to LOCK Left Bumper.",
		"LB"
	);
	ImGui::SetCursorPos(center - size * 0.5f + ImVec2(0.224f, -0.2f) * gamepadSize);
	renderBitmaskTextureButton(
		XUSB_GAMEPAD_RIGHT_SHOULDER, "Right Bumper", AppIcons::gamepadBumper.texture, size,
		"Right Bumper [Locked]", "Right Bumper [Unlocked]",
		"Press to UNLOCK Right Bumper.", "Press to LOCK Right Bumper.",
		"RB", true
	);

	// ===================================
	// DPad
	// ===================================
	size = ImVec2(AppIcons::gamepadDpadRight.width, AppIcons::gamepadDpadRight.height) * ratio;
	groupCenter = center - size * 0.5f + ImVec2(-0.78f, 1.0f) * gamepadSize * 0.14f;
	groupOffset = gamepadSize.y * 0.045f;
	ImGui::SetCursorPos(groupCenter + Vector2::right * groupOffset);
	renderBitmaskTextureButton(
		XUSB_GAMEPAD_DPAD_RIGHT, "D-Pad Right", AppIcons::gamepadDpadRight.texture, size,
		"D-Pad Right [Locked]", "D-Pad Right [Unlocked]",
		"Press to UNLOCK D-Pad Right.", "Press to LOCK D-Pad Right.",
		""
	);
	ImGui::SetCursorPos(groupCenter + Vector2::left * groupOffset);
	renderBitmaskTextureButton(
		XUSB_GAMEPAD_DPAD_LEFT, "D-Pad Left", AppIcons::gamepadDpadRight.texture, size,
		"D-Pad Left [Locked]", "D-Pad Left [Unlocked]",
		"Press to UNLOCK D-Pad Left.", "Press to LOCK D-Pad Left.",
		"", true
	);

	size = ImVec2(AppIcons::gamepadDpadUp.width, AppIcons::gamepadDpadUp.height) * ratio;
	ImGui::SetCursorPos(groupCenter - Vector2::up * groupOffset);
	renderBitmaskTextureButton(
		XUSB_GAMEPAD_DPAD_UP, "D-Pad Up", AppIcons::gamepadDpadUp.texture, size,
		"D-Pad Up [Locked]", "D-Pad Up [Unlocked]",
		"Press to UNLOCK D-Pad Up.", "Press to LOCK D-Pad Up.",
		""
	);
	ImGui::SetCursorPos(groupCenter - Vector2::down * groupOffset);
	renderBitmaskTextureButton(
		XUSB_GAMEPAD_DPAD_DOWN, "D-Pad Down", AppIcons::gamepadDpadUp.texture, size,
		"D-Pad Down [Locked]", "D-Pad Down [Unlocked]",
		"Press to UNLOCK D-Pad Down.", "Press to LOCK D-Pad Down.",
		"", false, true
	);


	// ===================================
	// Buttons
	// ===================================
	size = ImVec2(AppIcons::gamepadButton.width, AppIcons::gamepadButton.height) * ratio;
	groupCenter = center - size * 0.5f + ImVec2(1.0f, -0.1f) * gamepadSize * 0.25f;
	groupOffset = gamepadSize.y * 0.065f;
	ImGui::SetCursorPos(groupCenter + Vector2::right * groupOffset);
	renderBitmaskTextureButton(
		XUSB_GAMEPAD_B, "B", AppIcons::gamepadButton.texture, size,
		"B [Locked]", "B [Unlocked]",
		"Press to UNLOCK B.", "Press to LOCK B.",
		"B"
	);
	ImGui::SetCursorPos(groupCenter + Vector2::left * groupOffset);
	renderBitmaskTextureButton(
		XUSB_GAMEPAD_X, "X", AppIcons::gamepadButton.texture, size,
		"X [Locked]", "X [Unlocked]",
		"Press to UNLOCK X.", "Press to LOCK X.",
		"X"
	);

	size = ImVec2(AppIcons::gamepadButton.width, AppIcons::gamepadButton.height) * ratio;
	ImGui::SetCursorPos(groupCenter - Vector2::up * groupOffset);
	renderBitmaskTextureButton(
		XUSB_GAMEPAD_Y, "Y", AppIcons::gamepadButton.texture, size,
		"Y [Locked]", "Y [Unlocked]",
		"Press to UNLOCK Y.", "Press to LOCK Y.",
		"Y"
	);
	ImGui::SetCursorPos(groupCenter - Vector2::down * groupOffset);
	renderBitmaskTextureButton(
		XUSB_GAMEPAD_A, "A", AppIcons::gamepadButton.texture, size,
		"A [Locked]", "A [Unlocked]",
		"Press to UNLOCK A.", "Press to LOCK A.",
		"A"
	);

	// ===================================
	// Triggers
	// ===================================
	size = ImVec2(AppIcons::gamepadTrigger.width, AppIcons::gamepadTrigger.height) * ratio;
	ImGui::SetCursorPos(center - size * 0.5f + ImVec2(-0.28f, -0.385f) * gamepadSize);
	renderBoolTextureButton(
		MetadataCache::preferences.buttonLock.leftTrigger, "Left Trigger", AppIcons::gamepadTrigger.texture, size,
		"Left Trigger [Locked]", "Left Trigger [Unlocked]",
		"Press to UNLOCK Left Trigger.", "Press to LOCK Left Trigger.",
		"LT"
	);
	ImGui::SetCursorPos(center - size * 0.5f + ImVec2(0.28f, -0.385f) * gamepadSize);
	renderBoolTextureButton(
		MetadataCache::preferences.buttonLock.rightTrigger, "Right Trigger", AppIcons::gamepadTrigger.texture, size,
		"Right Trigger [Locked]", "Right Trigger [Unlocked]",
		"Press to UNLOCK Right Trigger.", "Press to LOCK Right Trigger.",
		"RT", true
	);

	// ===================================
	// Menu
	// ===================================
	size = ImVec2(AppIcons::gamepadMenu.width, AppIcons::gamepadMenu.height) * ratio;
	groupCenter = center - size * 0.5f + ImVec2(0.0f, -0.2f) * gamepadSize * 0.25f;
	groupOffset = gamepadSize.y * 0.11f;
	ImGui::SetCursorPos(groupCenter + Vector2::right * groupOffset);
	renderBitmaskTextureButton(
		XUSB_GAMEPAD_START, "Start", AppIcons::gamepadMenu.texture, size,
		"Start [Locked]", "Start [Unlocked]",
		"Press to UNLOCK Start.", "Press to LOCK Start.",
		""
	);
	ImGui::SetCursorPos(groupCenter + Vector2::left * groupOffset);
	renderBitmaskTextureButton(
		XUSB_GAMEPAD_BACK, "Back", AppIcons::gamepadMenu.texture, size,
		"Back [Locked]", "Back [Unlocked]",
		"Press to UNLOCK Back.", "Press to LOCK Back.",
		""
	);

	// ===================================
	// Guide
	// ===================================
	size = ImVec2(AppIcons::gamepadGuide.width, AppIcons::gamepadGuide.height) * ratio;
	ImGui::SetCursorPos(center - size * 0.5f + ImVec2(0.0f, 0.02f) * gamepadSize);
	renderBitmaskTextureButton(
		XUSB_GAMEPAD_GUIDE, "Guide", AppIcons::gamepadGuide.texture, size,
		"Guide [Locked]", "Guide [Unlocked]",
		"Press to UNLOCK Guide.", "Press to LOCK Guide.",
		""
	);


	// ===================================
	// Analogs
	// ===================================
	static float BASE_STICK_SIZE, BASE_THUMB_SIZE;
	BASE_STICK_SIZE = 32;
	BASE_THUMB_SIZE = 68;
	static float STICK_RADIUS, THUMB_RADIUS;
	STICK_RADIUS = BASE_STICK_SIZE * ratio * 0.5f;
	THUMB_RADIUS = BASE_THUMB_SIZE * ratio * 0.5f;
	groupCenter = center + ImVec2(-0.25f, -0.025f) * gamepadSize;
	ImGui::SetCursorPos(groupCenter);
	renderAnalog(
		MetadataCache::preferences.buttonLock.leftStick,
		XUSB_GAMEPAD_LEFT_THUMB, "Left", STICK_RADIUS, THUMB_RADIUS
	);

	groupCenter = center + ImVec2(0.105f, 0.14f) * gamepadSize;
	ImGui::SetCursorPos(groupCenter);
	renderAnalog(
		MetadataCache::preferences.buttonLock.rightStick,
		XUSB_GAMEPAD_RIGHT_THUMB, "Right", STICK_RADIUS, THUMB_RADIUS
	);
}

void ButtonLockWidget::renderBoolTextureButton(
	bool& state,
	const string& id, const Icon texture, const ImVec2& size,
	const char* tooltipTitleTrue, const char* tooltipTitleFalse,
	const char* tooltipDescriptionTrue, const char* tooltipDescriptionFalse,
	const string& label, const bool& flipX, const bool& flipY)
{
	if (renderToggleTextureButton(
		state, id, texture, size,
		tooltipTitleTrue, tooltipTitleFalse,
		tooltipDescriptionTrue, tooltipDescriptionFalse,
		label, flipX, flipY
	))
	{
		state = !state;
		MetadataCache::savePreferences();
	}
}

void ButtonLockWidget::renderBitmaskTextureButton(
	const unsigned int& bitMask,
	const string& id, const Icon texture, const ImVec2& size,
	const char* tooltipTitleTrue, const char* tooltipTitleFalse,
	const char* tooltipDescriptionTrue, const char* tooltipDescriptionFalse,
	const string& label, const bool& flipX, const bool& flipY
)
{
	static bool buttonState;
	buttonState = MetadataCache::preferences.buttonLock.isButtonLocked(bitMask);
	if (renderToggleTextureButton(
		buttonState, id, texture, size,
		tooltipTitleTrue, tooltipTitleFalse,
		tooltipDescriptionTrue, tooltipDescriptionFalse,
		label, flipX, flipY
	))
	{
		buttonState = !buttonState;
		MetadataCache::preferences.buttonLock.setButtonLocked(bitMask, buttonState);
		MetadataCache::savePreferences();
	}
}

bool ButtonLockWidget::renderToggleTextureButton(
	bool& state,
	const string& id, const Icon texture, const ImVec2& size,
	const char* tooltipTitleTrue, const char* tooltipTitleFalse,
	const char* tooltipDescriptionTrue, const char* tooltipDescriptionFalse,
	const string& label, const bool& flipX, const bool& flipY
)
{
	static bool isPressed;
	static ImVec2 uv0, uv1, cursorBefore, cursorNow, textSize;
	
	uv0.x = flipX ? 1 : 0;
	uv0.y = flipY ? 1 : 0;
	uv1.x = flipX ? 0 : 1;
	uv1.y = flipY ? 0 : 1;

	cursorBefore = ImGui::GetCursorPos();
	
	ImGui::PushID(("Button " + id).c_str());
	isPressed = ImGui::ImageButton(texture, size, uv0, uv1, 0, AppColors::invisible, state ? AppColors::red : AppColors::negative);
	ImGui::PopID();
	TitleTooltipWidget::renderBool(state, tooltipTitleTrue, tooltipTitleFalse, tooltipDescriptionTrue, tooltipDescriptionFalse);
	
	if (!label.empty())
	{
		cursorNow = ImGui::GetCursorPos();
		ImGui::SetCursorPos(cursorBefore);

		AppFonts::pushBigHeader();
		AppColors::pushColor(AppColors::disabled);
		textSize = ImGui::CalcTextSize(label.c_str());
		ImGui::SetCursorPos(cursorBefore + (size - textSize) * 0.5f);
		ImGui::Text(label.c_str());
		AppColors::pop();
		AppFonts::pop();

		ImGui::SetCursorPos(cursorNow);
	}

	return isPressed;
}

void ButtonLockWidget::renderAnalog(
	bool& stick, const unsigned short& thumbBitMask,
	const string& buttonName, const float& stickRadius, const float& thumbRadius
)
{
	static ImDrawList* drawList;
	drawList = ImGui::GetWindowDrawList();

	static ImVec2 cursor, groupCenter, size;
	cursor = ImGui::GetCursorScreenPos();

	static bool thumbState;
	thumbState = Bitwise::getBit(MetadataCache::preferences.buttonLock.buttons, thumbBitMask);

	static string finalName;
	static ImU32 inactiveColor = ImGui::GetColorU32(IM_COL32(104, 104, 115, 255));

	AppStyle::pushInput();
	drawList->AddCircle(cursor, thumbRadius, thumbState ? COLOR_RED_U32 : inactiveColor, 0, thumbRadius * 0.1f);
	drawList->AddCircleFilled(cursor, stickRadius, stick ? COLOR_RED_U32 : inactiveColor);
	AppStyle::pop();
	
	if (Mouse::isWithinCircle(cursor, 1.25f * thumbRadius))
	{
		if (Mouse::isWithinCircle(cursor, 0.75f * thumbRadius))
		{
			finalName = buttonName + " Analog";
			TitleTooltipWidget::renderBool(
				stick,
				(finalName + " [Locked]").c_str(), (finalName + " [Unlocked]").c_str(),
				("Press to UNLOCK " + finalName + ".").c_str(), ("Press to LOCK " + finalName + ".").c_str(),
				true
			);

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				stick = !stick;
				MetadataCache::savePreferences();
			}
		}
		else
		{
			finalName = buttonName + " Thumb";
			TitleTooltipWidget::renderBool(
				thumbState,
				(finalName + " [Locked]").c_str(), (finalName + " [Unlocked]").c_str(),
				("Press to UNLOCK " + finalName + ".").c_str(), ("Press to LOCK " + finalName + ".").c_str(),
				true
			);

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				thumbState = !thumbState;
				MetadataCache::preferences.buttonLock.setButtonLocked(thumbBitMask, thumbState);
				MetadataCache::savePreferences();
			}
		}
	}
}
