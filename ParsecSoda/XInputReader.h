#pragma once

#include <assert.h>
#include <mutex>
#include <roapi.h>
#include <vector>
#include <wrl.h>
#include "windows.gaming.input.h"

#include "ViGEm/Client.h"
#include "GamepadState.h"
#include "Bitwise.h"

using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::Gaming;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace std;

#pragma comment(lib, "runtimeobject.lib")

class XInputReader
{
public:
	void init();
	vector<GamepadState>& readInputs();

private:
	static GamepadState gamepadStateFromReading(Input::GamepadReading reading);
	static SHORT axisToShort(DOUBLE axis);
	static BYTE axisToByte(DOUBLE axis);
	static bool isButtonPressed(Input::GamepadReading& reading, Input::GamepadButtons button);
	static void setButton(GamepadState& padState, unsigned short bit, Input::GamepadReading& reading, Input::GamepadButtons button);

	bool _isReady = false;
	Input::IGamepadStatics* _gamepadStatistcs;
	mutex _mutex;

	vector<GamepadState> _gamepadReadings;
};

