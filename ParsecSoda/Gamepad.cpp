#include "Gamepad.h"

Gamepad::Gamepad()
	: parsec(nullptr)
{
	_client = nullptr;
	_isAlive = false;
	_index = GAMEPAD_INDEX_ERROR;
	_isConnected = false;
	clearOwner();
}

Gamepad::Gamepad(ParsecDSO* parsec, PVIGEM_CLIENT client)
	: parsec(parsec)
{
	_client = client;
	clearOwner();
	refreshIndex();
	_isConnected = false;

	alloc();
}

bool Gamepad::alloc()
{
	if (_client != nullptr)
	{
		pad = vigem_target_x360_alloc();

		vigem_target_set_vid(pad, 0x045E);
		vigem_target_set_pid(pad, 0x028E);
		_isAlive = true;
	}
	else
	{
		_isAlive = false;
	}

	return _isAlive;
}

bool Gamepad::realloc()
{
	release();
	return alloc();
}

bool Gamepad::connect()
{
	if (!_isAlive || _client == nullptr) { return false; }

	VIGEM_ERROR err = vigem_target_add(_client, pad);

	if (VIGEM_SUCCESS(err))
	{
		clearOwner();
		refreshIndex();
		_isConnected = true;

		const VIGEM_ERROR error = vigem_target_x360_register_notification(
			_client, pad,
			[](PVIGEM_CLIENT Client, PVIGEM_TARGET Target, UCHAR LargeMotor, UCHAR SmallMotor, UCHAR LedNumber, LPVOID UserData) {
				Gamepad* gamepad = reinterpret_cast<Gamepad*>(UserData);
				if (gamepad != nullptr)
				{
					if (gamepad->isConnected() && gamepad->isOwned() && gamepad->parsec != nullptr)
					{
						ParsecHostSubmitRumble(gamepad->parsec, gamepad->owner.guest.id, gamepad->owner.deviceID, LargeMotor, SmallMotor);
					}
				}
			},
			this
		);

		if (!VIGEM_SUCCESS(error))
		{
			bool stop = true;
		}

		return true;
	}

	return false;
}

bool Gamepad::disconnect()
{
	if (!_isAlive || _client == nullptr)
	{
		_isConnected = false;
		return false;
	}

	if (!VIGEM_SUCCESS(vigem_target_remove(_client, pad)))
	{
		return false;
	}

	_isConnected = false;
	clearOwner();
	return true;
}

void Gamepad::release()
{
	if (_isAlive)
	{
		disconnect();
		vigem_target_free(pad);
		clearOwner();
		_isAlive = false;
		_isConnected = false;
	}
}

bool Gamepad::isAttached()
{
	if (_isAlive)
	{
		return vigem_target_is_attached(pad);
	}

	return false;
}

bool Gamepad::refreshIndex()
{
	if (_isAlive)
	{
		VIGEM_ERROR err = vigem_target_x360_get_user_index(_client, pad, &_index);
		if (VIGEM_SUCCESS(err))
		{
			return _index;
		}
	}

	return _index;
}

void Gamepad::setIndex(ULONG index)
{
	_index = index;
}

ULONG Gamepad::getIndex() const
{
	return _index;
}

XINPUT_STATE Gamepad::getState()
{
	XINPUT_STATE state = {};

	if (_isAlive && _index >= 0)
	{
		XInputGetState(_index, &state);
	}

	return state;
}

void Gamepad::clearState()
{
	XINPUT_STATE xState;
	xState.Gamepad.wButtons = 0;
	xState.Gamepad.bLeftTrigger = 0;
	xState.Gamepad.bRightTrigger = 0;
	xState.Gamepad.sThumbLX = 0;
	xState.Gamepad.sThumbLY = 0;
	xState.Gamepad.sThumbRX = 0;
	xState.Gamepad.sThumbRY = 0;
	vigem_target_x360_update(_client, pad, *reinterpret_cast<XUSB_REPORT*>(&xState.Gamepad));
}

bool Gamepad::setState(ParsecGamepadStateMessage state)
{
	if (_isAlive && _isConnected && _client != nullptr)
	{
		XINPUT_STATE xState;
		xState.Gamepad.wButtons = state.buttons;
		xState.Gamepad.bLeftTrigger = state.leftTrigger;
		xState.Gamepad.bRightTrigger = state.rightTrigger;
		xState.Gamepad.sThumbLX = state.thumbLX;
		xState.Gamepad.sThumbLY = state.thumbLY;
		xState.Gamepad.sThumbRX = state.thumbRX;
		xState.Gamepad.sThumbRY = state.thumbRY;

		if (mirror)
		{
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_LEFT, state.thumbLX < -GAMEPAD_DEADZONE);
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, state.thumbLX > GAMEPAD_DEADZONE);
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_UP, state.thumbLY > GAMEPAD_DEADZONE);
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_DOWN, state.thumbLY < -GAMEPAD_DEADZONE);
		}

		vigem_target_x360_update(_client, pad, *reinterpret_cast<XUSB_REPORT*>(&xState.Gamepad));
		return true;
	}

	return false;
}

bool Gamepad::setState(ParsecKeyboardMessage key)
{
	if (_isAlive && _isConnected && _client != nullptr)
	{
		bool isOk = true;
		XINPUT_STATE xState = getState();

		switch (key.code)
		{

			// Directions
		case (int)KEY_TO_GAMEPAD::LEFT:
		case (int)KEY_TO_GAMEPAD2::LEFT:
			xState.Gamepad.sThumbLX = (key.pressed ? GAMEPAD_STICK_MIN : 0);
			if (mirror)
			{
				Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_LEFT, key.pressed);
			}
			break;
		case (int)KEY_TO_GAMEPAD::RIGHT:
		case (int)KEY_TO_GAMEPAD2::RIGHT:
			xState.Gamepad.sThumbLX = (key.pressed ? GAMEPAD_STICK_MAX : 0);
			if (mirror)
			{
				Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, key.pressed);
			}
			break;
		case (int)KEY_TO_GAMEPAD::UP:
		case (int)KEY_TO_GAMEPAD2::UP:
			xState.Gamepad.sThumbLY = (key.pressed ? GAMEPAD_STICK_MAX : 0);
			if (mirror)
			{
				Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_UP, key.pressed);
			}
			break;
		case (int)KEY_TO_GAMEPAD::DOWN:
		case (int)KEY_TO_GAMEPAD2::DOWN:
			xState.Gamepad.sThumbLY = (key.pressed ? GAMEPAD_STICK_MIN : 0);
			if (mirror)
			{
				Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_DOWN, key.pressed);
			}
			break;

			// Face buttons
		case (int)KEY_TO_GAMEPAD::A:
		case (int)KEY_TO_GAMEPAD2::A:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_A, key.pressed);
			break;
		case (int)KEY_TO_GAMEPAD::B:
		case (int)KEY_TO_GAMEPAD2::B:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_B, key.pressed);
			break;
		case (int)KEY_TO_GAMEPAD::X:
		case (int)KEY_TO_GAMEPAD2::X:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_X, key.pressed);
			break;
		case (int)KEY_TO_GAMEPAD::Y:
		case (int)KEY_TO_GAMEPAD2::Y:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_Y, key.pressed);
			break;

			// Center
		case (int)KEY_TO_GAMEPAD::BACK:
		case (int)KEY_TO_GAMEPAD2::BACK:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_BACK, key.pressed);
			break;
		case (int)KEY_TO_GAMEPAD::START:
		case (int)KEY_TO_GAMEPAD2::START:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_START, key.pressed);
			break;

			// Shoulders
		case (int)KEY_TO_GAMEPAD::LB:
		case (int)KEY_TO_GAMEPAD2::LB:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_LEFT_SHOULDER, key.pressed);
			break;
		case (int)KEY_TO_GAMEPAD::RB:
		case (int)KEY_TO_GAMEPAD2::RB:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_RIGHT_SHOULDER, key.pressed);
			break;

			// Triggers
		case (int)KEY_TO_GAMEPAD::LT:
		case (int)KEY_TO_GAMEPAD2::LT:
			xState.Gamepad.bLeftTrigger = (key.pressed ? GAMEPAD_STICK_MAX : GAMEPAD_STICK_MIN);
			break;
		case (int)KEY_TO_GAMEPAD::RT:
		case (int)KEY_TO_GAMEPAD2::RT:
			xState.Gamepad.bRightTrigger = (key.pressed ? GAMEPAD_STICK_MAX : GAMEPAD_STICK_MIN);
			break;

			// Thumbs
		case (int)KEY_TO_GAMEPAD::LTHUMB:
		case (int)KEY_TO_GAMEPAD2::LTHUMB:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_LEFT_THUMB, key.pressed);
			break;
		case (int)KEY_TO_GAMEPAD::RTHUMB:
		case (int)KEY_TO_GAMEPAD2::RTHUMB:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_RIGHT_THUMB, key.pressed);
			break;

		default:
			isOk = false;
			break;
		}

		if (isOk)
		{
			vigem_target_x360_update(_client, pad, *reinterpret_cast<XUSB_REPORT*>(&xState.Gamepad));
			return true;
		}
	}

	return false;
}

bool Gamepad::setState(ParsecGamepadButtonMessage button)
{
	if (_isAlive && _isConnected && _client != nullptr)
	{
		bool isOk = true;
		XINPUT_STATE xState = getState();

		int buttonCode = 0;

		switch (button.button)
		{
		case GAMEPAD_BUTTON_A:
			buttonCode = XUSB_GAMEPAD_A;
			break;
		case GAMEPAD_BUTTON_B:
			buttonCode = XUSB_GAMEPAD_B;
			break;
		case GAMEPAD_BUTTON_X:
			buttonCode = XUSB_GAMEPAD_X;
			break;
		case GAMEPAD_BUTTON_Y:
			buttonCode = XUSB_GAMEPAD_Y;
			break;
		case GAMEPAD_BUTTON_BACK:
			buttonCode = XUSB_GAMEPAD_BACK;
			break;
		case GAMEPAD_BUTTON_GUIDE:
			buttonCode = XUSB_GAMEPAD_GUIDE;
			break;
		case GAMEPAD_BUTTON_START:
			buttonCode = XUSB_GAMEPAD_START;
			break;
		case GAMEPAD_BUTTON_LSTICK:
			buttonCode = XUSB_GAMEPAD_LEFT_THUMB;
			break;
		case GAMEPAD_BUTTON_RSTICK:
			buttonCode = XUSB_GAMEPAD_RIGHT_THUMB;
			break;
		case GAMEPAD_BUTTON_LSHOULDER:
			buttonCode = XUSB_GAMEPAD_LEFT_SHOULDER;
			break;
		case GAMEPAD_BUTTON_RSHOULDER:
			buttonCode = XUSB_GAMEPAD_RIGHT_SHOULDER;
			break;
		case GAMEPAD_BUTTON_DPAD_UP:
			buttonCode = XUSB_GAMEPAD_DPAD_UP;
			break;
		case GAMEPAD_BUTTON_DPAD_DOWN:
			buttonCode = XUSB_GAMEPAD_DPAD_DOWN;
			break;
		case GAMEPAD_BUTTON_DPAD_LEFT:
			buttonCode = XUSB_GAMEPAD_DPAD_LEFT;
			break;
		case GAMEPAD_BUTTON_DPAD_RIGHT:
			buttonCode = XUSB_GAMEPAD_DPAD_RIGHT;
			break;
		default:
			isOk = false;
			break;
		}
		
		if (isOk)
		{
			Bitwise::setValue(&xState.Gamepad.wButtons, buttonCode, button.pressed);
			vigem_target_x360_update(_client, pad, *reinterpret_cast<XUSB_REPORT*>(&xState.Gamepad));
			
			return true;
		}
	}

	return false;
}

bool Gamepad::setState(ParsecGamepadAxisMessage axis)
{
	if (_isAlive && _isConnected && _client != nullptr)
	{
		cout << "Axis: " << axis.axis << " | " << axis.value;

		bool isOk = true;
		XINPUT_STATE xState = getState();

		switch (axis.axis)
		{
		case GAMEPAD_AXIS_LX:
			xState.Gamepad.sThumbLX = axis.value;
			if (mirror)
			{
				Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_LEFT, axis.value < -GAMEPAD_DEADZONE);
				Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, axis.value > GAMEPAD_DEADZONE);
			}
			break;
		case GAMEPAD_AXIS_LY:
			xState.Gamepad.sThumbLY = -axis.value;
			if (mirror)
			{
				Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_UP, axis.value < -GAMEPAD_DEADZONE);
				Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_DOWN, axis.value > GAMEPAD_DEADZONE);
			}
			break;
		case GAMEPAD_AXIS_RX:
			xState.Gamepad.sThumbRX = axis.value;
			break;
		case GAMEPAD_AXIS_RY:
			xState.Gamepad.sThumbRY = -axis.value;
			break;
		case GAMEPAD_AXIS_TRIGGERL:
			xState.Gamepad.bLeftTrigger = axis.value;
			break;
		case GAMEPAD_AXIS_TRIGGERR:
			xState.Gamepad.bRightTrigger = axis.value;
			break;
		default:
			isOk = false;
			break;
		}

		if (isOk)
		{
			vigem_target_x360_update(_client, pad, *reinterpret_cast<XUSB_REPORT*>(&xState.Gamepad));

			return true;
		}
	}

	return false;
}

void Gamepad::setOwner(Guest& guest, uint32_t deviceID, bool isKeyboard)
{
	owner.guest.copy(guest);
	owner.deviceID = deviceID;
	owner.isKeyboard = isKeyboard;
}

void Gamepad::copyOwner(Gamepad pad)
{
	owner.copy(pad.owner);
}

void Gamepad::clearOwner()
{
	clearState();
	owner = GuestDevice();
}

const bool Gamepad::isOwned()
{
	return owner.guest.isValid();
}

bool Gamepad::isConnected() const
{
	return _isConnected;
}