#include "XBoxGamepad.h"

XBoxGamepad::XBoxGamepad()
	: AGamepad()
{}

XBoxGamepad::XBoxGamepad(ParsecDSO* parsec, PVIGEM_CLIENT client)
	: AGamepad(parsec, client)
{}

bool XBoxGamepad::alloc()
{
	if (_client != nullptr)
	{
		_pad = vigem_target_x360_alloc();

		vigem_target_set_vid(_pad, 0x045E);
		vigem_target_set_pid(_pad, 0x028E);
		_isAlive = true;
	}
	else
	{
		_isAlive = false;
	}

	return _isAlive;
}

//bool XBoxGamepad::realloc()
//{
//	release();
//	return alloc();
//}

bool XBoxGamepad::connect()
{
	if (!_isAlive || _client == nullptr) { return false; }

	VIGEM_ERROR err = vigem_target_add(_client, _pad);

	if (VIGEM_SUCCESS(err))
	{
		clearOwner();
		refreshIndex();

		_isConnected = true;

		const VIGEM_ERROR error = vigem_target_x360_register_notification(
			_client, _pad,
			[](PVIGEM_CLIENT Client, PVIGEM_TARGET Target, UCHAR LargeMotor, UCHAR SmallMotor, UCHAR LedNumber, LPVOID UserData) {
				XBoxGamepad* gamepad = reinterpret_cast<XBoxGamepad*>(UserData);
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

//bool XBoxGamepad::disconnect()
//{
//	if (!_isAlive || _client == nullptr)
//	{
//		_isConnected = false;
//		return false;
//	}
//
//	if (!VIGEM_SUCCESS(vigem_target_remove(_client, _pad)))
//	{
//		return false;
//	}
//
//	_index = -1;
//	_isConnected = false;
//	clearOwner();
//	return true;
//}
//
//void XBoxGamepad::release()
//{
//	if (_isAlive)
//	{
//		disconnect();
//		vigem_target_free(_pad);
//		clearOwner();
//		_isAlive = false;
//		_isConnected = false;
//	}
//}

//bool XBoxGamepad::isAttached()
//{
//	if (_isAlive)
//	{
//		return vigem_target_is_attached(_pad);
//	}
//
//	return false;
//}

void XBoxGamepad::setState(XINPUT_STATE state)
{
	_currentState = state;
	vigem_target_x360_update(_client, _pad, *reinterpret_cast<XUSB_REPORT*>(&state.Gamepad));
}

void XBoxGamepad::setStateSafe(XINPUT_STATE state)
{
	if (_isAlive && _isConnected && _client != nullptr)
	{
		_currentState = state;
		vigem_target_x360_update(_client, _pad, *reinterpret_cast<XUSB_REPORT*>(&state.Gamepad));
	}
}

bool XBoxGamepad::refreshIndex()
{
	if (_isAlive)
	{
		_index = -1;

		clearState();
		XINPUT_STATE previousState = XINPUT_STATE(_currentState);
		XINPUT_STATE state = XINPUT_STATE();
		state.Gamepad.sThumbRX = 5 + Dice::number(15);
		state.Gamepad.sThumbRY = 5 + Dice::number(15);

		setState(state);

		XINPUT_STATE iState = XINPUT_STATE();
		for (size_t i = 0; i < XUSER_MAX_COUNT; ++i)
		{
			XInputGetState(i, &iState);
			if (iState.Gamepad.sThumbRX == state.Gamepad.sThumbRX && iState.Gamepad.sThumbRY == state.Gamepad.sThumbRY)
			{
				_index = i;
				break;
			}
		}

		clearState();
		setState(previousState);
	}

	return _index;
}

//void XBoxGamepad::setIndex(ULONG index)
//{
//	_index = index;
//}
//
//ULONG XBoxGamepad::getIndex() const
//{
//	return _index;
//}
//
//XINPUT_STATE XBoxGamepad::getState()
//{
//	return _currentState;
//}

//XINPUT_STATE XBoxGamepad::fetchXInputState()
//{
//	if (_isAlive && _index >= 0)
//	{
//		XInputGetState(_index, &_currentState);
//	}
//
//	return _currentState;
//}

void XBoxGamepad::clearState()
{
	_keyboard.clear();
	ZeroMemory(&_currentState, sizeof(XINPUT_STATE));
	vigem_target_x360_update(_client, _pad, *reinterpret_cast<XUSB_REPORT*>(&_currentState.Gamepad));
}

//XBoxGamepad::Keyboard& XBoxGamepad::getKeyboard()
//{
//	return _keyboard;
//}
//
//void XBoxGamepad::setOwner(Guest& guest, uint32_t deviceID, bool isKeyboard)
//{
//	owner.guest.copy(guest);
//	owner.deviceID = deviceID;
//	owner.isKeyboard = isKeyboard;
//}
//
//void XBoxGamepad::copyOwner(XBoxGamepad pad)
//{
//	owner.copy(pad.owner);
//}
//
//void XBoxGamepad::clearOwner()
//{
//	clearState();
//	owner = GuestDevice();
//}
//
//const bool XBoxGamepad::isOwned()
//{
//	return owner.guest.isValid();
//}
//
//bool XBoxGamepad::isConnected() const
//{
//	return _isConnected;
//}