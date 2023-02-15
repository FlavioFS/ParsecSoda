#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "../Stopwatch.h"
#include "../Debouncer.h"
#include "../Hosting.h"

#include "ProgressCircularWidget.h"
#include "VirtualGamepadWidget.h"
#include "AnimatedGamepadWidget.h"
#include "IconButton.h"
#include "IntRangeWidget.h"
#include "TitleTooltipWidget.h"
#include "ToggleIconButtonWidget.h"

class MasterOfPuppetsWidget
{
public:
	MasterOfPuppetsWidget(Hosting& hosting);
	bool render();

private:
	// Render engines
	void renderMaster();
	void renderMasterSDL();
	void renderMasterXInput();

	// UI control
	void setMaster(int value);
	void clearPuppets();
	void renderPuppets();

	MasterOfPuppets& _masterOfPuppets;
	GamepadClient& _gamepadClient;
	vector<AnimatedGamepadWidget> _masterGamepadAnimations;
	vector<AnimatedGamepadWidget> _puppetGamepadAnimations;

	// ==========================================
	// Tooltips
	// ==========================================
	const string _sdlTooltipString = string() +
		"Engine: SDL\n" +
		"\n" +
		"Pros:\n" +
		" + Detects all gamepads (XInput and DirectInput).\n" +
		" + Does not require window focus.\n" +
		"\n" +
		"Cons:\n" +
		" - Only pads in the 4 XInput slots can press both triggers at same time.\n" +
		" - Needs to choose manually between XBox and Dualshock button mapping.\n" +
		" - Sometimes detects additional ghost gamepads.\n" +
		" - DInput/Dualshock mapping treats triggers as buttons, not axis.\n" +
		"\n" +
		"* Click to switch to Windows Gaming Input engine.";

	const string _wgiTooltipString = string() +
		"Engine: Windows Gaming Input\n" +
		"\n" +
		"Pros:\n" +
		" + No button issues (triggers always work).\n" +
		" + Button mapping not required.\n" +
		"\n" +
		"Cons:\n" +
		" - Only detects XInput gamepads.\n" +
		" - Requires window focus (Parsec Soda must be focused).\n" +
		"\n" +
		"* Click to switch to SDL engine.";

	const string _masterTooltipBase = string() +
		"\n" +
		"The Master is a physical (or virtual) gamepad that controls virtual gamepads (the Puppets).\n" +
		"Master gamepad overwrites the Virtual Gamepads window, so it...\n" +
		" + Overwrites Guest inputs.\n" +
		" + Still works even when inputs are locked.\n" +
		" + Manipulates multiple Puppets at same time.\n"
		;
	const string _masterTooltipOn = "Master of Puppets is pulling the strings.\n" + _masterTooltipBase;
	const string _masterTooltipOff = "Click to set this gamepad as Master.\n" + _masterTooltipBase;

	const string _sdlXInputMapTooltip = string() +
		"Mapping type: XInput\n" +
		"\n" +
		"Cons:\n"
		" - Only fully supports 4 slots (including Puppets).\n" +
		" - XBox-like controlers above slot 4 cannot press both triggers at same time.\n" +
		"\n" +
		"* Click to switch to Dualshock type."
		;
	const string _sdlDualshockMapTooltip = string() +
		"Mapping type: Dualshock\n" +
		"\n" +
		"Cons:\n"
		" - DInput mapping treats triggers as buttons instead of axis (empty or full, no in-between).\n" +
		"\n" +
		"* Click to switch to Dualshock 4 type."
		;
	const string _sdlDS4MapTooltip = string() +
		"Mapping type: Dualshock 4\n" +
		"\n" +
		"This is 100%% experimental, I have no idea what is going to happen.\n" +
		"Hopefully, it will map correctly official DS4 gamepads connected to PC.\n" +
		"Use it at your own risk.\n" +
		"\n" +
		"* Click to switch to XInput type."
		;
};