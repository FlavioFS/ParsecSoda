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

	_index = -1;
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

void Gamepad::setState(XINPUT_STATE state)
{
	_currentState = state;
	vigem_target_x360_update(_client, pad, *reinterpret_cast<XUSB_REPORT*>(&state.Gamepad));
}

void Gamepad::setStateSafe(XINPUT_STATE state)
{
	if (_isAlive && _isConnected && _client != nullptr)
	{
		_currentState = state;
		vigem_target_x360_update(_client, pad, *reinterpret_cast<XUSB_REPORT*>(&state.Gamepad));
	}
}

bool Gamepad::refreshIndex()
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
	return _currentState;
}

XINPUT_STATE Gamepad::fetchXInputState()
{
	if (_isAlive && _index >= 0)
	{
		XInputGetState(_index, &_currentState);
	}

	return _currentState;
}

void Gamepad::clearState()
{
	_keyboard.clear();
	ZeroMemory(&_currentState, sizeof(XINPUT_STATE));
	vigem_target_x360_update(_client, pad, *reinterpret_cast<XUSB_REPORT*>(&_currentState.Gamepad));
}

Gamepad::Keyboard& Gamepad::getKeyboard()
{
	return _keyboard;
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