#include "Gamepad.h"


Gamepad::Gamepad()
{
	_client = nullptr;
	_isAlive = false;
	_index = GAMEPAD_INDEX_ERROR;
	_isConnected = false;
	setOwnerGuest();
}

Gamepad::Gamepad(PVIGEM_CLIENT client)
{
	_client = client;
	setOwnerGuest();
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
		getIndex();
		setOwnerGuest();
		_isConnected = true;
		return true;
	}

	disconnect();
	_isConnected = false;
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
	setOwnerGuest();
	return true;
}

void Gamepad::release()
{
	if (_isAlive)
	{
		disconnect();
		vigem_target_free(pad);
		setOwnerGuest();
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

ULONG Gamepad::getIndex()
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

XINPUT_STATE Gamepad::getState()
{
	XINPUT_STATE state = {};
	if (_isAlive && _index != GAMEPAD_INDEX_ERROR)
	{
		XInputGetState(_index, &state);
	}

	return state;
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
		xState.Gamepad.sThumbRY = state.thumbRX;

		vigem_target_x360_update(_client, pad, *reinterpret_cast<XUSB_REPORT*>(&xState.Gamepad));
		return true;
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
			if (buttons.pressed)
				xState.Gamepad.wButtons |= buttonCode;
			else
				xState.Gamepad.wButtons &= ~buttonCode;

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
		bool isOk = true;
		XINPUT_STATE xState = getState();

		switch (axis.axis)
		{
		case GAMEPAD_AXIS_LX:
			xState.Gamepad.sThumbLX = axis.value;
			break;
		case GAMEPAD_AXIS_LY:
			xState.Gamepad.sThumbLY = axis.value;
			break;
		case GAMEPAD_AXIS_RX:
			xState.Gamepad.sThumbRX = axis.value;
			break;
		case GAMEPAD_AXIS_RY:
			xState.Gamepad.sThumbRY = axis.value;
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

void Gamepad::setOwnerGuest(ParsecGuest guest, uint32_t padId)
{
	setOwnerGuest(guest.userID, guest.name, padId);
}

std::string Gamepad::getOwnerGuestName()
{
	return _ownerName;
}

uint32_t Gamepad::getOwnerGuestUserId()
{
	return _ownerId;
}

uint32_t Gamepad::getOwnerPadId()
{
	return _ownerPadId;
}

void Gamepad::onRageQuit()
{
	setOwnerGuest();
}

void Gamepad::setOwnerGuest(uint32_t ownerId, const char* ownerName, uint32_t padId)
{
	_ownerId = ownerId;
	_ownerName = ownerName;
	_ownerPadId = padId;
}