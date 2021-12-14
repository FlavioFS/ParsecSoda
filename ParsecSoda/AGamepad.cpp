#include "AGamepad.h"

AGamepad::AGamepad()
	: parsec(nullptr)
{
	_client = nullptr;
	_isAlive = false;
	_index = GAMEPAD_INDEX_ERROR;
	_isConnected = false;
	clearOwner();
}

AGamepad::AGamepad(ParsecDSO* parsec, PVIGEM_CLIENT client)
	: parsec(parsec)
{
	_client = client;
	clearOwner();
	refreshIndex();
	_isConnected = false;

	alloc();
}

//bool AGamepad::alloc()
//{
//	if (_client != nullptr)
//	{
//		_pad = vigem_target_x360_alloc();
//
//		vigem_target_set_vid(_pad, 0x045E);
//		vigem_target_set_pid(_pad, 0x028E);
//		_isAlive = true;
//	}
//	else
//	{
//		_isAlive = false;
//	}
//
//	return _isAlive;
//}

bool AGamepad::realloc()
{
	release();
	return alloc();
}

//bool AGamepad::connect()
//{
//	if (!_isAlive || _client == nullptr) { return false; }
//
//	VIGEM_ERROR err = vigem_target_add(_client, _pad);
//
//	if (VIGEM_SUCCESS(err))
//	{
//		clearOwner();
//		refreshIndex();
//
//		_isConnected = true;
//
//		const VIGEM_ERROR error = vigem_target_x360_register_notification(
//			_client, _pad,
//			[](PVIGEM_CLIENT Client, PVIGEM_TARGET Target, UCHAR LargeMotor, UCHAR SmallMotor, UCHAR LedNumber, LPVOID UserData) {
//				AGamepad* gamepad = reinterpret_cast<AGamepad*>(UserData);
//				if (gamepad != nullptr)
//				{
//					if (gamepad->isConnected() && gamepad->isOwned() && gamepad->parsec != nullptr)
//					{
//						ParsecHostSubmitRumble(gamepad->parsec, gamepad->owner.guest.id, gamepad->owner.deviceID, LargeMotor, SmallMotor);
//					}
//				}
//			},
//			this
//		);
//
//		if (!VIGEM_SUCCESS(error))
//		{
//			bool stop = true;
//		}
//
//		return true;
//	}
//
//	return false;
//}

bool AGamepad::disconnect()
{
	if (!_isAlive || _client == nullptr)
	{
		_isConnected = false;
		return false;
	}

	if (!VIGEM_SUCCESS(vigem_target_remove(_client, _pad)))
	{
		return false;
	}

	_index = -1;
	_isConnected = false;
	clearOwner();
	return true;
}

void AGamepad::release()
{
	if (_isAlive)
	{
		disconnect();
		vigem_target_free(_pad);
		clearOwner();
		_isAlive = false;
		_isConnected = false;
	}
}

bool AGamepad::isAttached()
{
	if (_isAlive)
	{
		return vigem_target_is_attached(_pad);
	}

	return false;
}

//void AGamepad::setState(XINPUT_STATE state)
//{
//	_currentState = state;
//	vigem_target_x360_update(_client, _pad, *reinterpret_cast<XUSB_REPORT*>(&state.Gamepad));
//}
//
//void AGamepad::setStateSafe(XINPUT_STATE state)
//{
//	if (_isAlive && _isConnected && _client != nullptr)
//	{
//		_currentState = state;
//		vigem_target_x360_update(_client, _pad, *reinterpret_cast<XUSB_REPORT*>(&state.Gamepad));
//	}
//}

bool AGamepad::refreshIndex()
{
	return false;
}

void AGamepad::setIndex(ULONG index)
{
	_index = index;
}

ULONG AGamepad::getIndex() const
{
	return _index;
}

XINPUT_STATE AGamepad::getState()
{
	return _currentState;
}

//XINPUT_STATE AGamepad::fetchXInputState()
//{
//	if (_isAlive && _index >= 0)
//	{
//		XInputGetState(_index, &_currentState);
//	}
//
//	return _currentState;
//}

//void AGamepad::clearState()
//{
//	_keyboard.clear();
//	ZeroMemory(&_currentState, sizeof(XINPUT_STATE));
//	vigem_target_x360_update(_client, _pad, *reinterpret_cast<XUSB_REPORT*>(&_currentState.Gamepad));
//}

AGamepad::Keyboard& AGamepad::getKeyboard()
{
	return _keyboard;
}

void AGamepad::setOwner(Guest& guest, uint32_t deviceID, bool isKeyboard)
{
	owner.guest.copy(guest);
	owner.deviceID = deviceID;
	owner.isKeyboard = isKeyboard;
}

void AGamepad::copyOwner(AGamepad* pad)
{
	if (pad != nullptr) owner.copy(pad->owner);
}

void AGamepad::clearOwner()
{
	clearState();
	owner = GuestDevice();
}

const bool AGamepad::isOwned()
{
	return owner.guest.isValid();
}

bool AGamepad::isConnected() const
{
	return _isConnected;
}