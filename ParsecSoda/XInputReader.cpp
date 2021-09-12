#include "XInputReader.h"

void XInputReader::init()
{
	if (_isReady) return;

	_mutex.lock();

	HRESULT hr = RoGetActivationFactory(
		HStringReference(L"Windows.Gaming.Input.Gamepad").Get(),
		__uuidof(Input::IGamepadStatics),
		(void**)&_gamepadStatistcs
	);

	_isReady = SUCCEEDED(hr);

	_mutex.unlock();
}

vector<GamepadState>& XInputReader::readInputs()
{
	_mutex.lock();

	HRESULT hr;
	bool success = false;
	IVectorView<Input::Gamepad*>* gamepads;
	uint32_t gamepadCount;

	if (!_isReady) goto BEFORE_QUIT;

	hr = _gamepadStatistcs->get_Gamepads(&gamepads);
	if (FAILED(hr)) goto BEFORE_QUIT;

	hr = gamepads->get_Size(&gamepadCount);
	if (FAILED(hr)) goto BEFORE_QUIT;

	_gamepadReadings.clear();
	for (uint32_t i = 0; i < gamepadCount; i++)
	{
		Input::IGamepad* gamepad;
		hr = gamepads->GetAt(i, &gamepad);
		if (SUCCEEDED(hr))
		{
			Input::GamepadReading gamepadReading;
			hr = gamepad->GetCurrentReading(&gamepadReading);
			if (SUCCEEDED(hr))
			{
				_gamepadReadings.push_back(gamepadStateFromReading(gamepadReading));
			}
		}
	}

BEFORE_QUIT:
	_mutex.unlock();

	return _gamepadReadings;
}

GamepadState XInputReader::gamepadStateFromReading(Input::GamepadReading reading)
{
	GamepadState padState;

	padState.state.Gamepad.bLeftTrigger = axisToByte(reading.LeftTrigger);
	padState.state.Gamepad.bRightTrigger = axisToByte(reading.RightTrigger);

	padState.state.Gamepad.sThumbLX = axisToShort(reading.LeftThumbstickX);
	padState.state.Gamepad.sThumbLY = axisToShort(reading.LeftThumbstickY);
	padState.state.Gamepad.sThumbRX = axisToShort(reading.RightThumbstickX);
	padState.state.Gamepad.sThumbRY = axisToShort(reading.RightThumbstickY);

	setButton(padState, XUSB_GAMEPAD_A, reading, Input::GamepadButtons_A);
	setButton(padState, XUSB_GAMEPAD_B, reading, Input::GamepadButtons_B);
	setButton(padState, XUSB_GAMEPAD_X, reading, Input::GamepadButtons_X);
	setButton(padState, XUSB_GAMEPAD_Y, reading, Input::GamepadButtons_Y);

	setButton(padState, XUSB_GAMEPAD_DPAD_UP,    reading, Input::GamepadButtons_DPadUp);
	setButton(padState, XUSB_GAMEPAD_DPAD_DOWN,  reading, Input::GamepadButtons_DPadDown);
	setButton(padState, XUSB_GAMEPAD_DPAD_LEFT,  reading, Input::GamepadButtons_DPadLeft);
	setButton(padState, XUSB_GAMEPAD_DPAD_RIGHT, reading, Input::GamepadButtons_DPadRight);

	setButton(padState, XUSB_GAMEPAD_BACK,  reading, Input::GamepadButtons_View);
	setButton(padState, XUSB_GAMEPAD_START, reading, Input::GamepadButtons_Menu);

	setButton(padState, XUSB_GAMEPAD_LEFT_SHOULDER,  reading, Input::GamepadButtons_LeftShoulder);
	setButton(padState, XUSB_GAMEPAD_RIGHT_SHOULDER, reading, Input::GamepadButtons_RightShoulder);

	setButton(padState, XUSB_GAMEPAD_LEFT_THUMB,  reading, Input::GamepadButtons_LeftThumbstick);
	setButton(padState, XUSB_GAMEPAD_RIGHT_THUMB, reading, Input::GamepadButtons_RightThumbstick);

	//static uint32_t GamepadButtons_Guide = 0x40000000;
	//Bitwise::setValue(&padState.state.Gamepad.wButtons, XUSB_GAMEPAD_GUIDE, (reading.Buttons & GamepadButtons_Guide) != 0);

	return padState;
}

SHORT XInputReader::axisToShort(DOUBLE axis)
{
	if (axis >= 0)
	{
		return (SHORT)(XINPUT_AXIS_MAX * axis);
	}

	return (SHORT)(XINPUT_AXIS_MIN * -axis);
}

BYTE XInputReader::axisToByte(DOUBLE axis)
{
	return (BYTE)(XINPUT_TRIGGER_MAX * axis);
}

bool XInputReader::isButtonPressed(Input::GamepadReading& reading, Input::GamepadButtons button)
{
	return (reading.Buttons & button) != 0;
}

void XInputReader::setButton(GamepadState& padState, unsigned short bit, Input::GamepadReading& reading, Input::GamepadButtons button)
{
	Bitwise::setValue(&padState.state.Gamepad.wButtons, bit, isButtonPressed(reading, button));
}
