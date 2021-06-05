#include "GamepadClient.h"

//////////////////////////////////////////////////////////////////
//
// GAMEPAD ENGINE
// 
//////////////////////////////////////////////////////////////////

GamepadClient::~GamepadClient()
{
	release();
}

bool GamepadClient::init()
{
	if (_client != nullptr)
	{
		release();
	}

	_client = vigem_alloc();
	if (_client == nullptr)
	{
		return false;
	}

	const VIGEM_ERROR err = vigem_connect(_client);
	if (!VIGEM_SUCCESS(err))
	{
		return false;
	}

	return true;
}

Gamepad GamepadClient::createGamepad()
{
	if (_client == nullptr || _gamepads.size() > XUSER_MAX_COUNT)
	{
	return nullptr;
	}

	Gamepad gamepad(_client);
	_gamepads.push_back(gamepad);

	return gamepad;
}

void GamepadClient::createMaximumGamepads()
{
	for (size_t i = 0; i < XUSER_MAX_COUNT; i++)
	{
		createGamepad();
	}
}

Gamepad GamepadClient::connectNextGamepad()
{
	std::vector<Gamepad>::iterator it;
	for (it = _gamepads.begin(); it != _gamepads.end(); ++it)
	{
		if ((*it).connect())
		{
			return *it;
		}
	}
	return Gamepad();
}

void GamepadClient::release()
{
	releaseGamepads();
	if (_client != nullptr)
	{
		vigem_disconnect(_client);
		vigem_free(_client);
		_client = nullptr;
	}
}

void GamepadClient::releaseGamepads()
{
	std::vector<Gamepad>::iterator it;
	for (it = _gamepads.begin(); it != _gamepads.end(); ++it)
	{
		(*it).release();
	}
_gamepads.clear();
}

Gamepad GamepadClient::getGamepad(int index)
{
	if (index >= 0 && index < _gamepads.size())
	{
		return _gamepads[index];
	}


	return Gamepad();
}

bool GamepadClient::sendMessage(ParsecGuest guest, ParsecMessage message)
{
	uint32_t padId = GAMEPAD_INDEX_ERROR;
	bool success = false;
	bool isGamepadRequest = false;

	std::vector<Gamepad>::iterator it = _gamepads.begin();
	switch (message.type)
	{
	case MESSAGE_GAMEPAD_STATE:
		padId = message.gamepadState.id;
		isGamepadRequest = isRequestState(message);
		for (; it != _gamepads.end(); ++it)
		{
			if (guest.userID == (*it).getOwnerGuestUserId() && message.gamepadState.id == (*it).getOwnerPadId())
			{
				(*it).setState(message.gamepadState);
				return true;
			}
		}
		break;
	case MESSAGE_GAMEPAD_AXIS:
		padId = message.gamepadAxis.id;
		for (; it != _gamepads.end(); ++it)
		{
			if (guest.userID == (*it).getOwnerGuestUserId() && message.gamepadAxis.id == (*it).getOwnerPadId())
			{
				(*it).setState(message.gamepadAxis);
				return true;
			}
		}
		break;
	case MESSAGE_GAMEPAD_BUTTON:
		padId = message.gamepadButton.id;
		isGamepadRequest = isRequestButton(message);
		for (; it != _gamepads.end(); ++it)
		{
			if (guest.userID == (*it).getOwnerGuestUserId() && message.gamepadButton.id == (*it).getOwnerPadId())
			{
				(*it).setState(message.gamepadButton);
				return true;
			}
		}
		break;
	default:
		break;
	}

	if (padId != GAMEPAD_INDEX_ERROR && isGamepadRequest)
	{
		success = tryAssignGamepad(guest, padId);
	}

	return success;
}

bool GamepadClient::tryAssignGamepad(ParsecGuest guest, uint32_t padId)
{
	GamepadClient::ParsecGuestPrefs* limit = getLimit(guest.userID);

	if (limit != nullptr && limit->slots >= limit->padLimit)
	{
		return false;
	}

	std::vector<Gamepad>::iterator it;
	for (it = _gamepads.begin(); it != _gamepads.end(); ++it)
	{
		if (!(*it).isAttached())
		{
			if ((*it).connect())
			{
				(*it).setOwnerGuest(guest, padId);
				if (limit != nullptr) { limit->slots++; }
				return true;
			}
		}
		else if ((*it).getOwnerGuestUserId() == OWNER_ID_NONE)
		{
			(*it).setOwnerGuest(guest, padId);
			if (limit != nullptr) { limit->slots++; }
			return true;
		}
	}

	return false;
}

int GamepadClient::onRageQuit(ParsecGuest guest)
{
	int result = 0;
	std::vector<Gamepad>::iterator it;
	for (it = _gamepads.begin(); it != _gamepads.end(); ++it)
	{
		if ((*it).getOwnerGuestUserId() == guest.userID)
		{
			(*it).onRageQuit();
			result++;
		}
	}
	freeSlots(guest.userID);
	return result;
}

void GamepadClient::setLimit(uint32_t guestUserId, uint8_t padLimit)
{
	std::vector<ParsecGuestPrefs>::iterator it;
	for (it = _guestPrefs.begin(); it != _guestPrefs.end(); ++it)
	{
		if ((*it).userId == guestUserId)
		{
			(*it).padLimit = padLimit;
			if ((*it).slots > padLimit)
			{
				refreshSlots(&(*it));
			}
			return;
		}
	}

	_guestPrefs.push_back({ guestUserId, padLimit });
}

void GamepadClient::setMirror(uint32_t guestUserId, bool mirror)
{
	std::vector<ParsecGuestPrefs>::iterator it;
	for (it = _guestPrefs.begin(); it != _guestPrefs.end(); ++it)
	{
		if ((*it).userId == guestUserId)
		{
			(*it).mirror = mirror;
			refreshSlots(&(*it));
			return;
		}
	}

	_guestPrefs.push_back({ guestUserId, XUSER_MAX_COUNT, mirror });
}

bool GamepadClient::toggleMirror(uint32_t guestUserId)
{
	std::vector<ParsecGuestPrefs>::iterator it;
	for (it = _guestPrefs.begin(); it != _guestPrefs.end(); ++it)
	{
		if ((*it).userId == guestUserId)
		{
			(*it).mirror = !(*it).mirror;
			refreshSlots(&(*it));
			return (*it).mirror;
		}
	}

	_guestPrefs.push_back({ guestUserId, XUSER_MAX_COUNT, true });
	return true;
}

void GamepadClient::freeSlots(uint32_t userId)
{
	std::vector<ParsecGuestPrefs>::iterator it;
	for (it = _guestPrefs.begin(); it != _guestPrefs.end(); ++it)
	{
		if ((*it).userId == userId)
		{
			(*it).slots = 0;
			return;
		}
	}
}

void GamepadClient::refreshSlots(ParsecGuestPrefs *prefs)
{
	prefs->slots = 0;

	std::vector<Gamepad>::iterator it;
	for (it = _gamepads.begin(); it != _gamepads.end(); ++it)
	{
		if ((*it).getOwnerGuestUserId() == prefs->userId)
		{
			if (prefs->slots >= prefs->padLimit)
			{
				(*it).onRageQuit();
			}
			else
			{
				(*it).setMirror(prefs->mirror);
				prefs->slots++;
			}
		}
	}
}

const std::vector<GamepadStatus> GamepadClient::getGamepadStatus()
{
	std::vector<GamepadStatus> result;

	std::vector<Gamepad>::iterator it = _gamepads.begin();
	for (; it != _gamepads.end(); ++it)
	{
		result.push_back((*it).getStatus());
	}

	return result;
}


bool GamepadClient::isRequestState(ParsecMessage message)
{
	return (message.gamepadState.buttons & (XUSB_GAMEPAD_A | XUSB_GAMEPAD_B | XUSB_GAMEPAD_X | XUSB_GAMEPAD_Y)) != 0;
}

bool GamepadClient::isRequestButton(ParsecMessage message)
{
	return message.gamepadButton.pressed && (
		message.gamepadButton.button == GAMEPAD_BUTTON_A ||
		message.gamepadButton.button == GAMEPAD_BUTTON_B ||
		message.gamepadButton.button == GAMEPAD_BUTTON_X ||
		message.gamepadButton.button == GAMEPAD_BUTTON_Y
	);
}

GamepadClient::ParsecGuestPrefs * GamepadClient::getLimit(uint32_t guestUserId)
{
	std::vector<ParsecGuestPrefs>::iterator it;
	for (it = _guestPrefs.begin(); it != _guestPrefs.end(); ++it)
	{
		if ((*it).userId == guestUserId)
		{
			return &(*it);
		}
	}

	return nullptr;
}
