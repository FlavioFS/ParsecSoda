#include "DualshockGamepad.h"

DualshockGamepad::DualshockGamepad()
	: AGamepad()
{
}

DualshockGamepad::DualshockGamepad(ParsecDSO* parsec, PVIGEM_CLIENT client)
	: AGamepad(parsec, client)
{
	alloc();
}

bool DualshockGamepad::alloc()
{
	if (_client != nullptr)
	{
		_pad = vigem_target_ds4_alloc();

		vigem_target_set_vid(_pad, 0x054C);
		vigem_target_set_pid(_pad, 0x05C4);
		_isAlive = true;
	}
	else
	{
		_isAlive = false;
	}

	return _isAlive;
}

bool DualshockGamepad::connect()
{
	if (!_isAlive || _client == nullptr) { return false; }

	VIGEM_ERROR err = vigem_target_add(_client, _pad);

	if (VIGEM_SUCCESS(err))
	{
		clearOwner();

		_isConnected = true;

		const VIGEM_ERROR error = vigem_target_ds4_register_notification(
			_client, _pad,
			[](PVIGEM_CLIENT Client, PVIGEM_TARGET Target, UCHAR LargeMotor, UCHAR SmallMotor, DS4_LIGHTBAR_COLOR LightbarColor, LPVOID UserData) {
				AGamepad* gamepad = reinterpret_cast<AGamepad*>(UserData);
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

void DualshockGamepad::clearState()
{
	_keyboard.clear();
	ZeroMemory(&_currentState, sizeof(XINPUT_STATE));
	vigem_target_ds4_update(_client, _pad, XInputToDS4(_currentState.Gamepad));
}

void DualshockGamepad::clearOwner()
{
	clearState();
	AGamepad::clearOwner();
}

bool DualshockGamepad::refreshIndex()
{
	return false;
}

void DualshockGamepad::setState(XINPUT_STATE state)
{
	_currentState = state;
	vigem_target_ds4_update(_client, _pad, XInputToDS4(state.Gamepad));
}

void DualshockGamepad::setStateSafe(XINPUT_STATE state)
{
	if (_isAlive && _isConnected && _client != nullptr)
	{
		_currentState = state;
		vigem_target_ds4_update(_client, _pad, XInputToDS4(state.Gamepad));
	}
}

DS4_REPORT DualshockGamepad::XInputToDS4(XINPUT_GAMEPAD& state)
{
	DS4_REPORT rv;
	DS4_REPORT_INIT(&rv);
	XUSB_TO_DS4_REPORT(reinterpret_cast<PXUSB_REPORT>(&state), &rv);

	return rv;
}