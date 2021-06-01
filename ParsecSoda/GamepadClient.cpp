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

	std::vector<Gamepad>::iterator it;
	for (it = _gamepads.begin(); it != _gamepads.end(); ++it)
	{
		switch (message.type)
		{
		case MESSAGE_GAMEPAD_STATE:
			padId = message.gamepadState.id;
			if (guest.userID == (*it).getOwnerGuestUserId() && message.gamepadState.id == (*it).getOwnerPadId())
			{
				(*it).setState(message.gamepadState);
				return true;
			}
			break;
		case MESSAGE_GAMEPAD_AXIS:
			padId = message.gamepadAxis.id;
			if (guest.userID == (*it).getOwnerGuestUserId() && message.gamepadAxis.id == (*it).getOwnerPadId())
			{
				if ((*it).setState(message.gamepadAxis));
				return true;
			}
			break;
		case MESSAGE_GAMEPAD_BUTTON:
			padId = message.gamepadButton.id;
			if (guest.userID == (*it).getOwnerGuestUserId() && message.gamepadButton.id == (*it).getOwnerPadId())
			{
				if ((*it).setState(message.gamepadButton));
				return true;
			}
			break;
		default:
			break;
		}
	}

	if (padId != GAMEPAD_INDEX_ERROR)
	{
		success = tryAssignGamepad(guest, padId);
	}

	return success;
}

bool GamepadClient::tryAssignGamepad(ParsecGuest guest, uint32_t padId)
{
	GamepadClient::ParsecGuestLimit* limit = getLimit(guest.userID);

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

void GamepadClient::onRageQuit(ParsecGuest guest)
{
	std::vector<Gamepad>::iterator it;
	for (it = _gamepads.begin(); it != _gamepads.end(); ++it)
	{
		if ((*it).getOwnerGuestUserId() == guest.userID)
		{
			(*it).onRageQuit();
		}
	}
}

void GamepadClient::setLimit(uint32_t guestUserId, uint8_t padLimit)
{
	std::vector<ParsecGuestLimit>::iterator it;
	for (it = _guestLimits.begin(); it != _guestLimits.end(); ++it)
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

	_guestLimits.push_back({ guestUserId, padLimit });
}

void GamepadClient::refreshSlots(ParsecGuestLimit *limit)
{
	limit->slots = 0;

	std::vector<Gamepad>::iterator it;
	for (it = _gamepads.begin(); it != _gamepads.end(); ++it)
	{
		if ((*it).getOwnerGuestUserId() == limit->userId)
		{
			if (limit->slots >= limit->padLimit)
			{
				(*it).onRageQuit();
			}
			else
			{
				limit->slots++;
			}
		}
	}
}

GamepadClient::ParsecGuestLimit * GamepadClient::getLimit(uint32_t guestUserId)
{
	std::vector<ParsecGuestLimit>::iterator it;
	for (it = _guestLimits.begin(); it != _guestLimits.end(); ++it)
	{
		if ((*it).userId == guestUserId)
		{
			return &(*it);
		}
	}

	return nullptr;
}
