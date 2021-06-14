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

void GamepadClient::connectAllGamepads()
{
	std::vector<Gamepad>::iterator it;
	for (it = _gamepads.begin(); it != _gamepads.end(); ++it)
	{
		(*it).connect();
	}
}

void GamepadClient::disconnectAllGamepads()
{
	std::vector<Gamepad>::iterator it;
	for (it = _gamepads.begin(); it != _gamepads.end(); ++it)
	{
		(*it).disconnect();
	}
}

void GamepadClient::sortGamepads()
{
	std::vector<Gamepad> sorted = _gamepads;
	std::sort(
		sorted.begin(),
		sorted.end(),
		[](const Gamepad a, const Gamepad b) {
			return a.getIndex() < b.getIndex();
		}
	);
	_gamepads = sorted;
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

bool GamepadClient::disconnect(int gamepadIndex)
{
	if (gamepadIndex < 0 || gamepadIndex > _gamepads.size())
	{
		return false;
	}
	bool rv = _gamepads[gamepadIndex].disconnect();
	return rv;
}

bool GamepadClient::clearOwner(int gamepadIndex)
{
	if (gamepadIndex < 0 || gamepadIndex > _gamepads.size())
	{
		_gamepads[gamepadIndex].clearOwner();
		return true;
	}
	return false;
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

int GamepadClient::clearAFK(GuestList &guests)
{
	int clearCount = 0;
	bool mustClear = true;
	std::vector<Gamepad>::iterator it;
	for (it = _gamepads.begin(); it != _gamepads.end(); ++it)
	{
		if ((*it).isOwned())
		{
			Guest guest;
			if (guests.find((*it).getOwner().userID, &guest))
			{
				(*it).clearOwner();
				clearCount++;
			}
		}
	}

	return clearCount;
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
			if (guest.userID == (*it).getOwner().userID && message.gamepadState.id == (*it).getOwnerPadId())
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
			if (guest.userID == (*it).getOwner().userID && message.gamepadAxis.id == (*it).getOwnerPadId())
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
			if (guest.userID == (*it).getOwner().userID && message.gamepadButton.id == (*it).getOwnerPadId())
			{
				(*it).setState(message.gamepadButton);
				return true;
			}
		}
		break;
	case MESSAGE_KEYBOARD:
		padId = GAMEPAD_KEYBOARD_PAD_ID;
		isGamepadRequest = isRequestKeyboard(message);
		for (; it != _gamepads.end(); ++it)
		{
			if (guest.userID == (*it).getOwner().userID && GAMEPAD_KEYBOARD_PAD_ID == (*it).getOwnerPadId())
			{
				(*it).setState(message.keyboard);
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
	GamepadClient::ParsecGuestPrefs* limit = getPrefs(guest.userID);

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
		else if ((*it).getOwner().userID == OWNER_ID_NONE)
		{
			(*it).setOwnerGuest(guest, padId);
			if (limit != nullptr) { limit->slots++; }
			return true;
		}
	}

	return false;
}

int GamepadClient::onRageQuit(Guest &guest)
{
	int result = 0;
	std::vector<Gamepad>::iterator it;
	for (it = _gamepads.begin(); it != _gamepads.end(); ++it)
	{
		if ((*it).getOwner().userID == guest.userID)
		{
			(*it).clearOwner();
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

const GAMEPAD_PICK_REQUEST GamepadClient::pick(Guest guest, int gamepadIndex)
{

	if (gamepadIndex < 0 || gamepadIndex >= _gamepads.size())
	{
		return GAMEPAD_PICK_REQUEST::OUT_OF_RANGE;
	}

	if (_gamepads[gamepadIndex].getOwner().userID == guest.userID)
	{
		return GAMEPAD_PICK_REQUEST::SAME_USER;
	}
	
	if (_gamepads[gamepadIndex].isOwned())
	{
		return GAMEPAD_PICK_REQUEST::TAKEN;
	}

	ParsecGuestPrefs *prefs = getPrefs(guest.userID);
	if (prefs != nullptr && prefs->padLimit <= 0)
	{
		return GAMEPAD_PICK_REQUEST::LIMIT_BLOCK;
	}

	std::vector<Gamepad>::iterator it;
	for (it = _gamepads.begin(); it != _gamepads.end(); ++it)
	{
		if ((*it).getOwner().userID == guest.userID)
		{
			_gamepads[gamepadIndex].connect();
			_gamepads[gamepadIndex].copyOwner(*it);
			(*it).clearOwner();
			return GAMEPAD_PICK_REQUEST::OK;
		}
	}

	return GAMEPAD_PICK_REQUEST::EMPTY_HANDS;
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
		if ((*it).getOwner().userID == prefs->userId)
		{
			if (prefs->slots >= prefs->padLimit)
			{
				(*it).clearOwner();
			}
			else
			{
				(*it).setMirror(prefs->mirror);
				prefs->slots++;
			}
		}
	}
}

std::vector<GamepadStatus> GamepadClient::getGamepadStatus()
{

	std::vector<GamepadStatus> result;

	std::vector<Gamepad>::iterator it = _gamepads.begin();
	for (; it != _gamepads.end(); ++it)
	{
		result.push_back((*it).getStatus());
	}

	return result;
}

vector<Gamepad>& GamepadClient::getGamepads()
{
	return _gamepads;
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

bool GamepadClient::isRequestKeyboard(ParsecMessage message)
{
	return message.keyboard.pressed && (
		message.keyboard.code == (int)KEY_TO_GAMEPAD::A ||
		message.keyboard.code == (int)KEY_TO_GAMEPAD::B ||
		message.keyboard.code == (int)KEY_TO_GAMEPAD::X ||
		message.keyboard.code == (int)KEY_TO_GAMEPAD::Y ||
		message.keyboard.code == (int)KEY_TO_GAMEPAD2::A ||
		message.keyboard.code == (int)KEY_TO_GAMEPAD2::B ||
		message.keyboard.code == (int)KEY_TO_GAMEPAD2::X ||
		message.keyboard.code == (int)KEY_TO_GAMEPAD2::Y
	);
}

bool GamepadClient::find(uint32_t userID, uint32_t padID, Gamepad *gamepad)
{
	vector<Gamepad>::iterator gi = _gamepads.begin();
	for (; gi != _gamepads.end(); ++gi)
	{
		if ((*gi).getOwner().userID == userID && (*gi).getOwnerPadId() == padID)
		{
			gamepad = &(*gi);
			return true;
		}
	}

	return false;
}

bool GamepadClient::findFirst(uint32_t userID, Gamepad *gamepad)
{
	vector<Gamepad>::iterator gi = _gamepads.begin();
	for (; gi != _gamepads.end(); ++gi)
	{
		if ((*gi).getOwner().userID == userID)
		{
			gamepad = &(*gi);
			return true;
		}
	}

	return false;
}

bool GamepadClient::findAll(uint32_t userID, vector<Gamepad*> &gamepads)
{
	vector<Gamepad>::iterator gi = _gamepads.begin();
	for (; gi != _gamepads.end(); ++gi)
	{
		if ((*gi).getOwner().userID == userID)
		{
			gamepads.push_back(&(*gi));
		}
	}

	return true;
}

GamepadClient::ParsecGuestPrefs * GamepadClient::getPrefs(uint32_t guestUserId)
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
