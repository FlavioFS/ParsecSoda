#include "Gamepad.h"


Gamepad::Gamepad()
{
	_client = nullptr;
	_isAlive = false;
	_index = GAMEPAD_INDEX_ERROR;
	_isConnected = false;
	clearOwner();
}

Gamepad::Gamepad(PVIGEM_CLIENT client)
{
	_client = client;
	clearOwner();
	_index = GAMEPAD_INDEX_ERROR;
	_isConnected = false;

	if (_client != nullptr)
	{
		pad = vigem_target_x360_alloc();
		_isAlive = true;
	}
	else
	{
		_isAlive = false;
	}
}

bool Gamepad::connect()
{
	if (!_isAlive || _client == nullptr) { return false; }

	VIGEM_ERROR err = vigem_target_add(_client, pad);
	if (VIGEM_SUCCESS(err))
	{
		refreshIndex();
		clearOwner();
		_isConnected = true;
		return true;
	}

	//disconnect();
	//_isConnected = false;
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

	_index = GAMEPAD_INDEX_ERROR;
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
		_index = GAMEPAD_INDEX_ERROR;
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
		_index = GAMEPAD_INDEX_ERROR;
		VIGEM_ERROR err = vigem_target_x360_get_user_index(_client, pad, &_index);
		if (VIGEM_SUCCESS(err))
		{
			return _index;
		}
	}

	_index = GAMEPAD_INDEX_ERROR;
	return _index;
}

ULONG Gamepad::getIndex() const
{
	return _index;
}

XINPUT_STATE Gamepad::getState()
{
	XINPUT_STATE state = {};
	if (_isAlive && _index != GAMEPAD_INDEX_ERROR)
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

		if (_mirror)
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

bool Gamepad::setState(ParsecKeyboardMessage keys)
{
	if (_isAlive && _isConnected && _client != nullptr)
	{
		bool isOk = true;
		XINPUT_STATE xState = getState();

		switch (keys.code)
		{

			// Directions
		case (int)KEY_TO_GAMEPAD::LEFT:
		case (int)KEY_TO_GAMEPAD2::LEFT:
			xState.Gamepad.sThumbLX = (keys.pressed ? GAMEPAD_STICK_MIN : 0);
			if (_mirror)
			{
				Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_LEFT, keys.pressed);
			}
			break;
		case (int)KEY_TO_GAMEPAD::RIGHT:
		case (int)KEY_TO_GAMEPAD2::RIGHT:
			xState.Gamepad.sThumbLX = (keys.pressed ? GAMEPAD_STICK_MAX : 0);
			if (_mirror)
			{
				Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, keys.pressed);
			}
			break;
		case (int)KEY_TO_GAMEPAD::UP:
		case (int)KEY_TO_GAMEPAD2::UP:
			xState.Gamepad.sThumbLY = (keys.pressed ? GAMEPAD_STICK_MAX : 0);
			if (_mirror)
			{
				Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_UP, keys.pressed);
			}
			break;
		case (int)KEY_TO_GAMEPAD::DOWN:
		case (int)KEY_TO_GAMEPAD2::DOWN:
			xState.Gamepad.sThumbLY = (keys.pressed ? GAMEPAD_STICK_MIN : 0);
			if (_mirror)
			{
				Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_DOWN, keys.pressed);
			}
			break;

			// Face buttons
		case (int)KEY_TO_GAMEPAD::A:
		case (int)KEY_TO_GAMEPAD2::A:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_A, keys.pressed);
			break;
		case (int)KEY_TO_GAMEPAD::B:
		case (int)KEY_TO_GAMEPAD2::B:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_B, keys.pressed);
			break;
		case (int)KEY_TO_GAMEPAD::X:
		case (int)KEY_TO_GAMEPAD2::X:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_X, keys.pressed);
			break;
		case (int)KEY_TO_GAMEPAD::Y:
		case (int)KEY_TO_GAMEPAD2::Y:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_Y, keys.pressed);
			break;

			// Center
		case (int)KEY_TO_GAMEPAD::BACK:
		case (int)KEY_TO_GAMEPAD2::BACK:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_BACK, keys.pressed);
			break;
		case (int)KEY_TO_GAMEPAD::START:
		case (int)KEY_TO_GAMEPAD2::START:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_START, keys.pressed);
			break;

			// Shoulders
		case (int)KEY_TO_GAMEPAD::LB:
		case (int)KEY_TO_GAMEPAD2::LB:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_LEFT_SHOULDER, keys.pressed);
			break;
		case (int)KEY_TO_GAMEPAD::RB:
		case (int)KEY_TO_GAMEPAD2::RB:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_RIGHT_SHOULDER, keys.pressed);
			break;

			// Triggers
		case (int)KEY_TO_GAMEPAD::LT:
		case (int)KEY_TO_GAMEPAD2::LT:
			xState.Gamepad.bLeftTrigger = (keys.pressed ? GAMEPAD_STICK_MAX : GAMEPAD_STICK_MIN);
			break;
		case (int)KEY_TO_GAMEPAD::RT:
		case (int)KEY_TO_GAMEPAD2::RT:
			xState.Gamepad.bRightTrigger = (keys.pressed ? GAMEPAD_STICK_MAX : GAMEPAD_STICK_MIN);
			break;

			// Thumbs
		case (int)KEY_TO_GAMEPAD::LTHUMB:
		case (int)KEY_TO_GAMEPAD2::LTHUMB:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_LEFT_THUMB, keys.pressed);
			break;
		case (int)KEY_TO_GAMEPAD::RTHUMB:
		case (int)KEY_TO_GAMEPAD2::RTHUMB:
			Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_RIGHT_THUMB, keys.pressed);
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

bool Gamepad::setState(ParsecGamepadButtonMessage buttons)
{
	if (_isAlive && _isConnected && _client != nullptr)
	{
		bool isOk = true;
		XINPUT_STATE xState = getState();

		int buttonCode = 0;

		switch (buttons.button)
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
			Bitwise::setValue(&xState.Gamepad.wButtons, buttonCode, buttons.pressed);
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
		std::cout << "Axis: " << axis.axis << " | " << axis.value;

		bool isOk = true;
		XINPUT_STATE xState = getState();

		switch (axis.axis)
		{
		case GAMEPAD_AXIS_LX:
			xState.Gamepad.sThumbLX = axis.value;
			if (_mirror)
			{
				Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_LEFT, axis.value < -GAMEPAD_DEADZONE);
				Bitwise::setValue(&xState.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, axis.value > GAMEPAD_DEADZONE);
			}
			break;
		case GAMEPAD_AXIS_LY:
			xState.Gamepad.sThumbLY = -axis.value;
			if (_mirror)
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

void Gamepad::setOwnerGuest(ParsecGuest guest, uint32_t padId, bool mirror)
{
	setOwnerGuest(guest.userID, guest.name, padId, mirror);
}

void Gamepad::copyOwner(Gamepad pad)
{
	setOwnerGuest(pad._ownerUserId, pad._ownerName.c_str(), pad._ownerPadId, pad._mirror);
}

void Gamepad::clearOwner()
{
	setOwnerGuest();
	clearState();
}

const bool Gamepad::isOwned()
{
	return _ownerUserId != OWNER_ID_NONE;
}

std::string Gamepad::getOwnerGuestName()
{
	return _ownerName;
}

uint32_t Gamepad::getOwnerGuestUserId()
{
	return _ownerUserId;
}

uint32_t Gamepad::getOwnerPadId()
{
	return _ownerPadId;
}

void Gamepad::onRageQuit()
{
	clearOwner();
}

void Gamepad::setMirror(bool mirror)
{
	_mirror = mirror;
}

GamepadStatus Gamepad::getStatus()
{
	return { _ownerUserId, _ownerName, _isConnected };
}

void Gamepad::setOwnerGuest(uint32_t ownerId, const char* ownerName, uint32_t padId, bool mirror)
{
	_ownerUserId = ownerId;
	_ownerName = ownerName;
	_ownerPadId = padId;
	setMirror(mirror);
}