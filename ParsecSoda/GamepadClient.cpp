#include "GamepadClient.h"

// =============================================================
//
//  GAMEPAD ENGINE
// 
// =============================================================

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
	if (_client == nullptr || gamepads.size() > XUSER_MAX_COUNT)
	{
		return nullptr;
	}

	Gamepad gamepad(_client);
	gamepads.push_back(gamepad);
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
	reduce([](Gamepad& pad) {
		pad.connect();
	});
}

void GamepadClient::disconnectAllGamepads()
{
	reduce([](Gamepad& pad) {
		pad.disconnect();
	});
}

void GamepadClient::sortGamepads()
{
	std::vector<Gamepad> sorted = gamepads;
	std::sort(
		sorted.begin(),
		sorted.end(),
		[](const Gamepad a, const Gamepad b) {
			return a.getIndex() < b.getIndex();
		}
	);
	gamepads = sorted;
}


Gamepad GamepadClient::connectNextGamepad()
{
	Gamepad rv;

	bool success = reduceUntilFirst([&](Gamepad& pad) {
		if (pad.connect())
		{
			rv = pad;
			return true;
		}
		return false;
	});

	if (success)
	{
		return rv;
	}
	
	return Gamepad();
}

bool GamepadClient::disconnect(int gamepadIndex)
{
	if (gamepadIndex < 0 || gamepadIndex > gamepads.size())
	{
		return false;
	}
	bool rv = gamepads[gamepadIndex].disconnect();
	return rv;
}

bool GamepadClient::clearOwner(int gamepadIndex)
{
	if (gamepadIndex >= 0 || gamepadIndex < gamepads.size())
	{
		gamepads[gamepadIndex].clearOwner();
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


Gamepad GamepadClient::getGamepad(int index)
{
	if (index >= 0 && index < gamepads.size())
	{
		return gamepads[index];
	}

	return Gamepad();
}

int GamepadClient::clearAFK(GuestList &guests)
{
	int clearCount = 0;

	reduce([&](Gamepad& pad) {
		if (pad.isOwned())
		{
			Guest guest;
			if (!guests.find(pad.owner.guest.userID, &guest))
			{
				pad.clearOwner();
				clearCount++;
			}
		}
	});

	return clearCount;
}

int GamepadClient::onQuit(Guest& guest)
{
	int result = 0;

	reduce([&](Gamepad& gamepad) {
		if (gamepad.owner.guest.userID == guest.userID)
		{
			gamepad.clearOwner();
			result++;
		}
	});

	return result;
}

void GamepadClient::setLimit(uint32_t guestUserID, uint8_t padLimit)
{
	bool found = findPreferences(guestUserID, [&padLimit](GuestPreferences& prefs) {
		prefs.padLimit = padLimit;
	});

	if (!found)
	{
		guestPreferences.push_back(
			GuestPreferences(guestUserID, padLimit)
		);
	}
}

bool GamepadClient::toggleMirror(uint32_t guestUserID)
{
	bool currentValue = false;

	bool found = findPreferences(guestUserID, [&currentValue](GuestPreferences& prefs) {
		prefs.mirror = !prefs.mirror;
		currentValue = prefs.mirror;
	});

	if(!found)
	{
		GuestPreferences prefs = GuestPreferences(guestUserID, 1, true);
		guestPreferences.push_back(prefs);
		currentValue = true;
	}

	return currentValue;
}

const GamepadClient::PICK_REQUEST GamepadClient::pick(Guest guest, int gamepadIndex)
{

	if (gamepadIndex < 0 || gamepadIndex >= gamepads.size())
	{
		return PICK_REQUEST::OUT_OF_RANGE;
	}

	if (gamepads[gamepadIndex].owner.guest.userID == guest.userID)
	{
		return PICK_REQUEST::SAME_USER;
	}

	if (gamepads[gamepadIndex].isOwned())
	{
		return PICK_REQUEST::TAKEN;
	}

	GuestPreferences prefs = GuestPreferences(guest.userID);
	int limit = 1;
	bool found = findPreferences(guest.userID, [&limit](GuestPreferences& prefs) {
		limit = prefs.padLimit;
	});

	if (limit <= 0)
	{
		return PICK_REQUEST::LIMIT_BLOCK;
	}

	bool success = reduceUntilFirst([&](Gamepad& gamepad) {
		if (gamepad.owner.guest.userID == guest.userID)
		{
			gamepads[gamepadIndex].connect();
			gamepads[gamepadIndex].copyOwner(gamepad);
			gamepad.clearOwner();
			return true;
		}

		return false;
	});

	if (success)
	{
		return PICK_REQUEST::OK;
	}

	return PICK_REQUEST::EMPTY_HANDS;
}


// =============================================================
//
//  Private
//
// =============================================================

bool GamepadClient::sendMessage(Guest guest, ParsecMessage message)
{
	bool success = false;
	uint32_t padId = GAMEPAD_INDEX_ERROR;
	bool isGamepadRequest = false;
	int slots = 0;

	switch (message.type)
	{
	case MESSAGE_GAMEPAD_STATE:
		padId = message.gamepadState.id;
		isGamepadRequest = isRequestState(message);
		if (sendGamepadStateMessage(message.gamepadState, guest, slots)) { return true; }
		break;

	case MESSAGE_GAMEPAD_AXIS:
		padId = message.gamepadAxis.id;
		if (sendGamepadAxisMessage(message.gamepadAxis, guest, slots)) { return true; }
		break;

	case MESSAGE_GAMEPAD_BUTTON:
		padId = message.gamepadButton.id;
		isGamepadRequest = isRequestButton(message);
		if (sendGamepadButtonMessage(message.gamepadButton, guest, slots)) { return true; }
		break;

	case MESSAGE_KEYBOARD:
		padId = 0;
		isGamepadRequest = isRequestKeyboard(message);
		if (sendKeyboardMessage(message.keyboard, guest, slots)) { return true; }
		break;

	default:
		break;
	}

	if (!success && isGamepadRequest)
	{
		success = tryAssignGamepad(guest, padId, slots, message.type == MESSAGE_KEYBOARD);
	}

	return success;
}

bool GamepadClient::sendGamepadStateMessage(ParsecGamepadStateMessage& gamepadState, Guest& guest, int &slots)
{
	return reduceUntilFirst([&](Gamepad& pad) {
	if (guest.userID == pad.owner.guest.userID)
	{
		slots++;
		if (gamepadState.id == pad.owner.deviceID)
		{
			pad.setState(gamepadState);
			return true;
		}
	}
	return false;
	});
}

bool GamepadClient::sendGamepadAxisMessage(ParsecGamepadAxisMessage& gamepadAxis, Guest& guest, int& slots)
{
	return reduceUntilFirst([&](Gamepad& pad) {
		if (guest.userID == pad.owner.guest.userID)
		{
			slots++;
			if (gamepadAxis.id == pad.owner.deviceID)
			{
				pad.setState(gamepadAxis);
				return true;
			}
		}
		return false;
	});
}

bool GamepadClient::sendGamepadButtonMessage(ParsecGamepadButtonMessage& gamepadButton, Guest& guest, int& slots)
{
	return reduceUntilFirst([&](Gamepad& pad) {
		if (guest.userID == pad.owner.guest.userID)
		{
			slots++;
			if (gamepadButton.id == pad.owner.deviceID)
			{
				pad.setState(gamepadButton);
				return true;
			}
		}
		return false;
	});
}

bool GamepadClient::sendKeyboardMessage(ParsecKeyboardMessage& keyboard, Guest& guest, int& slots)
{
	return reduceUntilFirst([&](Gamepad& pad) {
		if (guest.userID == pad.owner.guest.userID)
		{
			slots++;
			if (pad.owner.isKeyboard)
			{
				pad.setState(keyboard);
				return true;
			}
		}
		return false;
		});
}

bool GamepadClient::tryAssignGamepad(Guest guest, uint32_t deviceID, int currentSlots, bool isKeyboard)
{
	int limit = 1;
	findPreferences(guest.userID, [&limit](GuestPreferences& prefs) {
		limit = prefs.padLimit;
	});

	if (currentSlots >= limit)
	{
		return false;
	}
	
	return reduceUntilFirst([&](Gamepad& gamepad) {
		if (!gamepad.isAttached())
		{
			if (gamepad.connect())
			{
				gamepad.setOwner(guest, deviceID, isKeyboard);
				return true;
			}
		}
		else if (!gamepad.owner.guest.isValid())
		{
			gamepad.setOwner(guest, deviceID, isKeyboard);
			return true;
		}

		return false;
	});
}

void GamepadClient::releaseGamepads()
{
	reduce([](Gamepad& pad) {
		pad.release();
	});

	gamepads.clear();
}

void GamepadClient::setMirror(uint32_t guestUserID, bool mirror)
{
	GuestPreferences prefs = GuestPreferences(guestUserID, 1, mirror);

	bool found = findPreferences(guestUserID, [&mirror](GuestPreferences& prefs) {
		prefs.mirror = mirror;
	});

	if (!found)
	{
		guestPreferences.push_back(prefs);
	}
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

void GamepadClient::reduce(function<void(Gamepad&)> func)
{
	vector<Gamepad>::iterator gi = gamepads.begin();
	for (; gi != gamepads.end(); ++gi)
	{
		func(*gi);
	}
}

bool GamepadClient::reduceUntilFirst(function<bool(Gamepad&)> func)
{
	vector<Gamepad>::iterator gi = gamepads.begin();
	for (; gi != gamepads.end(); ++gi)
	{
		if (func(*gi))
		{
			return true;
		}
	}

	return false;
}

bool GamepadClient::findPreferences(uint32_t guestUserID, function<void(GamepadClient::GuestPreferences&)> callback)
{
	std::vector<GuestPreferences>::iterator it;
	for (it = guestPreferences.begin(); it != guestPreferences.end(); ++it)
	{
		if ((*it).userID == guestUserID)
		{
			callback(*it);
			return true;
		}
	}

	return false;
}
