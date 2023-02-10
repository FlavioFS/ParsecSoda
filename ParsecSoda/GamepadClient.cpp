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

void GamepadClient::setParsec(ParsecDSO* parsec)
{
	this->_parsec = parsec;
}

void GamepadClient::setHotseatManager(HotseatManager* hotSeatManager)
{
	this->_hotseatManager = hotSeatManager;
}

bool GamepadClient::init()
{
	_keyboardMap = KeyboardMap::defaultMap();

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

AGamepad* GamepadClient::createGamepad(AGamepad::Type type)
{
	if (_client == nullptr)
	{
		return nullptr;
	}

	AGamepad* gamepad = nullptr;
	switch (type)
	{
	case AGamepad::Type::DUALSHOCK:
		gamepad = new DualshockGamepad(_parsec, _client);
		break;
	default:
	case AGamepad::Type::XBOX:
		gamepad = new XBoxGamepad(_parsec, _client);
		break;
	}
	
	if (gamepad != nullptr)
		gamepads.push_back(gamepad);

	return gamepad;
}

void GamepadClient::createAllGamepads()
{
	_isBusy = true;

	for (uint16_t i = 0; i < MetadataCache::preferences.xboxPuppetCount; i++)
	{
		this->createGamepad(AGamepad::Type::XBOX);
	}

	for (uint16_t i = 0; i < MetadataCache::preferences.ds4PuppetCount; i++)
	{
		this->createGamepad(AGamepad::Type::DUALSHOCK);
	}

	sortGamepads();
	_isBusy = false;
	Sleep(200);
}

void GamepadClient::connectAllGamepads()
{
	reduce([](AGamepad* pad) {
		pad->connect();
		Sleep(200);
	});
}

void GamepadClient::disconnectAllGamepads()
{
	reduce([](AGamepad* pad) {
		pad->disconnect();
	});
}

void GamepadClient::sortGamepads()
{
	std::vector<AGamepad*> sorted = gamepads;
	std::sort(
		sorted.begin(),
		sorted.end(),
		[](const AGamepad* a, const AGamepad* b) {
			if (a->type() == AGamepad::Type::DUALSHOCK && b->type() == AGamepad::Type::XBOX) return false;
			if (a->type() == AGamepad::Type::XBOX && b->type() == AGamepad::Type::DUALSHOCK) return true;
			return a->getIndex() < b->getIndex();
		}
	);
	gamepads = sorted;
}

void GamepadClient::resize(size_t xboxCount, size_t dualshockCount)
{
	if (_isBusy) return;

	size_t xi = 0, di = 0;
	vector<AGamepad*> newGamepads;

	lock = true;
	_isBusy = true;

	reduce([&xi, &di, &xboxCount, &dualshockCount, &newGamepads](AGamepad* pad) {
		switch (pad->type())
		{
		case AGamepad::Type::XBOX:
			if (xi < xboxCount) {
				newGamepads.push_back(pad);
				xi++;
			}
			else {
				pad->release();
			}
			break;
		case AGamepad::Type::DUALSHOCK:
			if (di < dualshockCount) {
				newGamepads.push_back(pad);
				di++;
			}
			else {
				pad->release();
			}
			break;
		default:
			break;
		}
	});

	if (xi < xboxCount)
	{
		for (size_t i = 0; i < xboxCount-xi; i++)
		{
			newGamepads.push_back(this->createGamepad(AGamepad::Type::XBOX));
		}
	}
	if (di < dualshockCount)
	{
		for (size_t i = 0; i < dualshockCount-di; i++)
		{
			newGamepads.push_back(this->createGamepad(AGamepad::Type::DUALSHOCK));
		}
	}

	gamepads.clear();
	gamepads = newGamepads;
	sortGamepads();

	_isBusy = false;
	lock = false;
}

void GamepadClient::resetAll()
{
	_resetAllThread = thread([&]() {
		lock = true;
		release();
		init();
		createAllGamepads();
		connectAllGamepads();
		lock = false;
		_resetAllThread.detach();
	});
}

void GamepadClient::toggleLock()
{
	lock = !lock;
	reduce([&](AGamepad* pad) {
		pad->clearState();
	});
}


AGamepad* GamepadClient::connectNextGamepad()
{
	AGamepad* rv;

	bool success = reduceUntilFirst([&](AGamepad* pad) {
		if (pad->connect())
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
	
	return nullptr;
}

bool GamepadClient::disconnect(int gamepadIndex)
{
	if (gamepadIndex < 0 || gamepadIndex > gamepads.size())
	{
		return false;
	}

	bool rv = gamepads[gamepadIndex]->disconnect();
	return rv;
}

bool GamepadClient::clearOwner(int gamepadIndex)
{
	if (gamepadIndex >= 0 || gamepadIndex < gamepads.size())
	{
		gamepads[gamepadIndex]->clearOwner();
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


AGamepad* GamepadClient::getGamepad(int index)
{
	if (index >= 0 && index < gamepads.size())
	{
		return gamepads[index];
	}

	return nullptr;
}

int GamepadClient::clearAFK(GuestList &guests)
{
	int clearCount = 0;

	reduce([&](AGamepad* pad) {
		if (pad->isOwned())
		{
			if (!guests.findSafe(pad->owner.guest.userID))
			{
				pad->clearOwner();
				clearCount++;
			}
		}
	});

	return clearCount;
}

void GamepadClient::loadFromHotseats(GuestList& guests)
{
	if (!isHotseatsActive()) return;

	const vector<Hotseat> seats = _hotseatManager->getSeats();
	vector<Hotseat>::const_iterator iSeat = seats.begin();

	for (; iSeat != seats.end(); ++iSeat)
	{
		if (iSeat->guest.isMaster)
		{
			iSeat->gamepad->clearOwner();
			iSeat->gamepad->isPuppet = true;
		}
		else
		{
			guests.findUnsafe(iSeat->guest.guest.userID, [&](Guest* iGuest) {
				iSeat->gamepad->setOwner(*iGuest, iSeat->guest.deviceID, iSeat->guest.isKeyboard);
				iSeat->gamepad->isPuppet = false;
			});
		}

	}
}

int GamepadClient::onQuit(Guest& guest)
{
	int result = 0;

	reduce([&](AGamepad* gamepad) {
		if (gamepad->owner.guest.userID == guest.userID)
		{
			gamepad->clearOwner();
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
		prefs.toggleMirror();
		currentValue = prefs.isMirror();
	});

	if(!found)
	{
		GuestPreferences prefs = GuestPreferences(guestUserID, 1, false);
		guestPreferences.push_back(prefs);
		currentValue = prefs.isMirror();
	}

	return currentValue;
}

bool GamepadClient::toggleMultitap(uint32_t guestUserID)
{
	bool currentValue = false;

	bool found = findPreferences(guestUserID, [&currentValue](GuestPreferences& prefs) {
		prefs.toggleMultitap();
		currentValue = prefs.isMultitap();
	});

	if (!found)
	{
		GuestPreferences prefs = GuestPreferences(guestUserID, 1, true, false);
		guestPreferences.push_back(prefs);
		currentValue = prefs.isMultitap();
	}

	return currentValue;
}

const GamepadClient::PICK_REQUEST GamepadClient::pick(Guest guest, int gamepadIndex)
{

	if (gamepadIndex < 0 || gamepadIndex >= gamepads.size())
	{
		return PICK_REQUEST::OUT_OF_RANGE;
	}

	AGamepad* pad = gamepads[gamepadIndex];
	if (!pad->isConnected())
	{
		return PICK_REQUEST::DISCONNECTED;
	}

	if (pad->owner.guest.userID == guest.userID)
	{
		return PICK_REQUEST::SAME_USER;
	}

	if (pad->isOwned())
	{
		return PICK_REQUEST::TAKEN;
	}

	if (isPuppetMaster && pad->isPuppet)
	{
		return PICK_REQUEST::PUPPET;
	}

	GuestPreferences prefs = GuestPreferences(guest.userID);
	int limit = 4;
	bool found = findPreferences(guest.userID, [&limit](GuestPreferences& prefs) {
		limit = prefs.padLimit;
	});

	if (limit <= 0)
	{
		return PICK_REQUEST::LIMIT_BLOCK;
	}

	bool success = reduceUntilFirst([&](AGamepad* gamepad) {
		if (gamepad->owner.guest.userID == guest.userID)
		{
			pad->clearState();
			pad->copyOwner(gamepad);
			gamepad->clearOwner();
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
	
	GuestPreferences guestPrefs = GuestPreferences();
	findPreferences(guest.userID, [&guestPrefs](GuestPreferences& prefs) {
		guestPrefs = prefs;
	});

	switch (message.type)
	{
	case MESSAGE_GAMEPAD_STATE:
		padId = message.gamepadState.id;
		isGamepadRequest = isRequestState(message);
		if (sendGamepadStateMessage(message.gamepadState, guest, slots, guestPrefs)) { return true; }
		break;

	case MESSAGE_GAMEPAD_AXIS:
		padId = message.gamepadAxis.id;
		if (sendGamepadAxisMessage(message.gamepadAxis, guest, slots, guestPrefs)) { return true; }
		break;

	case MESSAGE_GAMEPAD_BUTTON:
		padId = message.gamepadButton.id;
		isGamepadRequest = isRequestButton(message);
		if (sendGamepadButtonMessage(message.gamepadButton, guest, slots, guestPrefs)) { return true; }
		break;

	case MESSAGE_KEYBOARD:
		padId = 0;
		isGamepadRequest = isRequestKeyboard(message);
		if (sendKeyboardMessage(message.keyboard, guest, slots, guestPrefs)) { return true; }
		break;

	default:
		break;
	}

	if (!success && isGamepadRequest)
	{
		if (isHotseatsActive())
		{
			success = enqueueHotseatRequest(guest, padId, slots, message.type == MESSAGE_KEYBOARD, guestPrefs);
		}
		else
		{
			success = tryAssignGamepad(guest, padId, slots, message.type == MESSAGE_KEYBOARD, guestPrefs);
		}
	}

	return success;
}

bool GamepadClient::sendGamepadStateMessage(ParsecGamepadStateMessage& gamepadState, Guest& guest, int &slots, GuestPreferences prefs)
{
	return reduceUntilFirst([&](AGamepad* pad) {
		if (guest.userID == pad->owner.guest.userID)
		{
			slots++;
			if (!(isPuppetMaster && pad->isPuppet) && !pad->isLocked() && (!prefs.isMultitap() || gamepadState.id == pad->owner.deviceID))
			{
				pad->setStateSafe(toXInput(gamepadState, pad, prefs));
				return true;
			}
		}
		return false;
	});
}

bool GamepadClient::sendGamepadAxisMessage(ParsecGamepadAxisMessage& gamepadAxis, Guest& guest, int& slots, GuestPreferences prefs)
{
	return reduceUntilFirst([&](AGamepad* pad) {
		if (guest.userID == pad->owner.guest.userID)
		{
			slots++;
			if (!(isPuppetMaster && pad->isPuppet) && !pad->isLocked() && (!prefs.isMultitap() || gamepadAxis.id == pad->owner.deviceID))
			{
				pad->setStateSafe(toXInput(gamepadAxis, pad->getState(), prefs));
				return true;
			}
		}
		return false;
	});
}

bool GamepadClient::sendGamepadButtonMessage(ParsecGamepadButtonMessage& gamepadButton, Guest& guest, int& slots, GuestPreferences prefs)
{
	return reduceUntilFirst([&](AGamepad* pad) {
		if (guest.userID == pad->owner.guest.userID)
		{
			slots++;
			if (!(isPuppetMaster && pad->isPuppet) && !pad->isLocked() && (!prefs.isMultitap() || gamepadButton.id == pad->owner.deviceID))
			{
				pad->setStateSafe(toXInput(gamepadButton, pad->getState(), prefs));
				return true;
			}
		}
	return false;
		});
}

bool GamepadClient::sendKeyboardMessage(ParsecKeyboardMessage& keyboard, Guest& guest, int& slots, GuestPreferences prefs)
{
	return reduceUntilFirst([&](AGamepad* pad) {
		if (guest.userID == pad->owner.guest.userID)
		{
			slots++;
			if (!(isPuppetMaster && pad->isPuppet) && !pad->isLocked() && (!prefs.isMultitap() || pad->owner.isKeyboard))
			{
				pad->setStateSafe(toXInput(keyboard, pad->getKeyboard(), pad->getState(), prefs));
				return true;
			}
		}
		return false;
	});
}

bool GamepadClient::tryAssignGamepad(Guest guest, uint32_t deviceID, int currentSlots, bool isKeyboard, GuestPreferences prefs)
{
	if (currentSlots >= prefs.padLimit)
	{
		return false;
	}
	
	return reduceUntilFirst([&](AGamepad* gamepad) {
		if (!(isPuppetMaster && gamepad->isPuppet) && (!gamepad->isLocked() && gamepad->isAttached() && !gamepad->owner.guest.isValid()))
		{
			gamepad->setOwner(guest, deviceID, isKeyboard);
			return true;
		}

		return false;
	});
}

bool GamepadClient::enqueueHotseatRequest(Guest guest, uint32_t deviceID, int currentSlots, bool isKeyboard, GuestPreferences prefs)
{
	if (currentSlots >= prefs.padLimit)
	{
		return false;
	}

	_hotseatManager->enqueue(HotseatGuest(guest, deviceID, isKeyboard, false, true));
}

void GamepadClient::releaseGamepads()
{
	reduce([](AGamepad* pad) {
		pad->release();
	});

	gamepads.clear();
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
		message.keyboard.code == _keyboardMap.A ||
		message.keyboard.code == _keyboardMap.B ||
		message.keyboard.code == _keyboardMap.X ||
		message.keyboard.code == _keyboardMap.Y
	);
}

void GamepadClient::reduce(function<void(AGamepad*)> func)
{
	vector<AGamepad*>::iterator gi = gamepads.begin();
	for (; gi != gamepads.end(); ++gi)
	{
		func(*gi);
	}
}

bool GamepadClient::reduceUntilFirst(function<bool(AGamepad*)> func)
{
	vector<AGamepad*>::iterator gi = gamepads.begin();
	for (; gi != gamepads.end(); ++gi)
	{
		if (func(*gi))
		{
			return true;
		}
	}

	return false;
}

bool GamepadClient::isHotseatsActive()
{
	return _hotseatManager != nullptr && _hotseatManager->isEnabled();
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

bool GamepadClient::isBusy()
{
	return _isBusy;
}

void GamepadClient::toggleLockGamepad(int index)
{
	if (index >= 0 && index < gamepads.size())
	{
		gamepads[index]->toggleLocked();
	}
}


XINPUT_STATE GamepadClient::toXInput(ParsecGamepadStateMessage& state, AGamepad* pad, GuestPreferences& prefs)
{
	XINPUT_STATE rawState = pad->getState();
	XINPUT_STATE result = rawState;
	XINPUT_STATE mirrorCache = pad->getMirrorCache();

	rawState.Gamepad.wButtons = state.buttons;
	rawState.Gamepad.bLeftTrigger = state.leftTrigger;
	rawState.Gamepad.bRightTrigger = state.rightTrigger;
	rawState.Gamepad.sThumbLX = state.thumbLX;
	rawState.Gamepad.sThumbLY = state.thumbLY;
	rawState.Gamepad.sThumbRX = state.thumbRX;
	rawState.Gamepad.sThumbRY = state.thumbRY;

	// Block guide button
	if (MetadataCache::preferences.disableGuideButton)
	{
		Bitwise::setValue(rawState.Gamepad.wButtons, XUSB_GAMEPAD_GUIDE, false);
	}

	if (prefs.isMirror())
	{	
		result = isDpadEqual(mirrorCache, rawState) ? stickToDpad(rawState) : dpadToStick(rawState);
		pad->setMirrorCache(rawState);
	}
	else
	{
		result = rawState;
	}

	return result;
}

XINPUT_STATE GamepadClient::toXInput(ParsecGamepadButtonMessage& button, const XINPUT_STATE& previousState, GuestPreferences& prefs)
{
	XINPUT_STATE result = previousState;
	
	int buttonCode = 0;

	switch (button.button)
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
		if (!MetadataCache::preferences.disableGuideButton)
		{
			buttonCode = XUSB_GAMEPAD_GUIDE;
		}
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
		if (prefs.isMirror()) setAxisFromSign(result.Gamepad.sThumbLY, 1);
		break;
	case GAMEPAD_BUTTON_DPAD_DOWN:
		buttonCode = XUSB_GAMEPAD_DPAD_DOWN;
		if (prefs.isMirror()) setAxisFromSign(result.Gamepad.sThumbLY, -1);
		break;
	case GAMEPAD_BUTTON_DPAD_LEFT:
		buttonCode = XUSB_GAMEPAD_DPAD_LEFT;
		if (prefs.isMirror()) setAxisFromSign(result.Gamepad.sThumbLX, -1);
		break;
	case GAMEPAD_BUTTON_DPAD_RIGHT:
		buttonCode = XUSB_GAMEPAD_DPAD_RIGHT;
		if (prefs.isMirror()) setAxisFromSign(result.Gamepad.sThumbLX, 1);
		break;
	default:
		break;
	}

	Bitwise::setValue(result.Gamepad.wButtons, buttonCode, button.pressed);

	return result;
}

XINPUT_STATE GamepadClient::toXInput(ParsecGamepadAxisMessage& axis, const XINPUT_STATE& previousState, GuestPreferences& prefs)
{
	XINPUT_STATE result = previousState;

	switch (axis.axis)
	{
	case GAMEPAD_AXIS_LX:
		result.Gamepad.sThumbLX = axis.value;
		if (prefs.isMirror())
		{
			setDpadFromAxis(result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, XUSB_GAMEPAD_DPAD_LEFT, result.Gamepad.sThumbLX);
		}
		break;
	case GAMEPAD_AXIS_LY:
		result.Gamepad.sThumbLY = -axis.value;
		if (prefs.isMirror())
		{
			setDpadFromAxis(result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_UP, XUSB_GAMEPAD_DPAD_DOWN, result.Gamepad.sThumbLY);
		}
		break;
	case GAMEPAD_AXIS_RX:
		result.Gamepad.sThumbRX = axis.value;
		break;
	case GAMEPAD_AXIS_RY:
		result.Gamepad.sThumbRY = -axis.value;
		break;
	case GAMEPAD_AXIS_TRIGGERL:
		result.Gamepad.bLeftTrigger = axis.value;
		break;
	case GAMEPAD_AXIS_TRIGGERR:
		result.Gamepad.bRightTrigger = axis.value;
		break;
	default:
		break;
	}

	return result;
}

XINPUT_STATE GamepadClient::toXInput(
	ParsecKeyboardMessage& key, AGamepad::Keyboard& keyboard,
	const XINPUT_STATE& previousState, GuestPreferences& prefs
)
{
	XINPUT_STATE result = previousState;

	// Left Stick
	if (key.code == _keyboardMap.LLeft)
	{
		if (key.pressed) setAxisFromSign(result.Gamepad.sThumbLX, -1);
		else setAxisFromSign(result.Gamepad.sThumbLX, keyboard.LRight ? 1 : 0);
		if (prefs.isMirror()) fetchDpadFromAxisX(result);
		keyboard.LLeft = key.pressed;
	}

	if (key.code == _keyboardMap.LRight)
	{
		if (key.pressed) setAxisFromSign(result.Gamepad.sThumbLX, 1);
		else setAxisFromSign(result.Gamepad.sThumbLX, keyboard.LLeft ? -1 : 0);
		if (prefs.isMirror()) fetchDpadFromAxisX(result);
		keyboard.LRight = key.pressed;
	}

	if (key.code == _keyboardMap.LUp)
	{
		if (key.pressed) setAxisFromSign(result.Gamepad.sThumbLY, 1);
		else setAxisFromSign(result.Gamepad.sThumbLY, keyboard.LDown ? -1 : 0);
		if (prefs.isMirror()) fetchDpadFromAxisY(result);
		keyboard.LUp = key.pressed;
	}

	if (key.code == _keyboardMap.LDown)
	{
		if (key.pressed) setAxisFromSign(result.Gamepad.sThumbLY, -1);
		else setAxisFromSign(result.Gamepad.sThumbLY, keyboard.LUp ? 1 : 0);
		if (prefs.isMirror()) fetchDpadFromAxisY(result);
		keyboard.LDown = key.pressed;
	}

	// Right Stick
	if (key.code == _keyboardMap.RLeft)
	{
		if (key.pressed) setAxisFromSign(result.Gamepad.sThumbRX, -1);
		else setAxisFromSign(result.Gamepad.sThumbRX, keyboard.RRight ? 1 : 0);
		keyboard.RLeft = key.pressed;
	}

	if (key.code == _keyboardMap.RRight)
	{
		if (key.pressed) setAxisFromSign(result.Gamepad.sThumbRX, 1);
		else setAxisFromSign(result.Gamepad.sThumbRX, keyboard.RLeft ? -1 : 0);
		keyboard.RRight = key.pressed;
	}

	if (key.code == _keyboardMap.RUp)
	{
		if (key.pressed) setAxisFromSign(result.Gamepad.sThumbRY, 1);
		else setAxisFromSign(result.Gamepad.sThumbRY, keyboard.RDown ? -1 : 0);
		keyboard.RUp = key.pressed;
	}

	if (key.code == _keyboardMap.RDown)
	{
		if (key.pressed) setAxisFromSign(result.Gamepad.sThumbRY, -1);
		else setAxisFromSign(result.Gamepad.sThumbRY, keyboard.RUp ? 1 : 0);
		keyboard.RDown = key.pressed;
	}


	// DPad
	if (key.code == _keyboardMap.DLeft)
	{
		Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_LEFT, key.pressed);
		Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, key.pressed ? false : keyboard.DRight);
		keyboard.DLeft = key.pressed;
		if (prefs.isMirror()) fecthAxisFromDpadX(result);
	}
	if (key.code == _keyboardMap.DRight)
	{
		Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, key.pressed);
		Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_LEFT, key.pressed ? false : keyboard.DLeft);
		keyboard.DRight = key.pressed;
		if (prefs.isMirror()) fecthAxisFromDpadX(result);
	}
	if (key.code == _keyboardMap.DUp)
	{
		Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_UP, key.pressed);
		Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_DOWN, key.pressed ? false : keyboard.DDown);
		keyboard.DUp = key.pressed;
		if (prefs.isMirror()) fecthAxisFromDpadY(result);
	}
	if (key.code == _keyboardMap.DDown)
	{
		Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_DOWN, key.pressed);
		Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_UP, key.pressed ? false : keyboard.DUp);
		keyboard.DDown = key.pressed;
		if (prefs.isMirror()) fecthAxisFromDpadY(result);
	}

	// Face buttons
	if (key.code == _keyboardMap.A) Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_A, key.pressed);
	if (key.code == _keyboardMap.B) Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_B, key.pressed);
	if (key.code == _keyboardMap.X) Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_X, key.pressed);
	if (key.code == _keyboardMap.Y) Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_Y, key.pressed);

	// Center
	if (key.code == _keyboardMap.Back)  Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_BACK, key.pressed);
	if (key.code == _keyboardMap.Start) Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_START, key.pressed);

	// Shoulders
	if (key.code == _keyboardMap.LB) Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_LEFT_SHOULDER, key.pressed);
	if (key.code == _keyboardMap.RB) Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_RIGHT_SHOULDER, key.pressed);

	// Triggers
	if (key.code == _keyboardMap.LT) result.Gamepad.bLeftTrigger = (key.pressed ? GAMEPAD_SHORT_MAX : GAMEPAD_SHORT_MIN);
	if (key.code == _keyboardMap.RT) result.Gamepad.bRightTrigger = (key.pressed ? GAMEPAD_SHORT_MAX : GAMEPAD_SHORT_MIN);

	// Thumbs
	if (key.code == _keyboardMap.LThumb) Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_LEFT_THUMB, key.pressed);
	if (key.code == _keyboardMap.RThumb) Bitwise::setValue(result.Gamepad.wButtons, XUSB_GAMEPAD_RIGHT_THUMB, key.pressed);

	return result;
}

int GamepadClient::getDpadSignX(const WORD& wButtons)
{
	return getDpadSign(wButtons, XUSB_GAMEPAD_DPAD_RIGHT, XUSB_GAMEPAD_DPAD_LEFT);
}

int GamepadClient::getDpadSignY(const WORD& wButtons)
{
	return getDpadSign(wButtons, XUSB_GAMEPAD_DPAD_UP, XUSB_GAMEPAD_DPAD_DOWN);
}

int GamepadClient::getDpadSign(const WORD& wButtons, const WORD padDirPositive, const WORD padDirNegative)
{
	bool pos = Bitwise::getBit(wButtons, padDirPositive);
	bool neg = Bitwise::getBit(wButtons, padDirNegative);

	if (pos && !neg) return 1;
	if (!pos && neg) return -1;
	return 0;
}

void GamepadClient::setAxisFromSign(SHORT& axis, int sign)
{
	if (sign > 0) axis = GAMEPAD_SHORT_MAX;
	else if (sign < 0) axis = GAMEPAD_SHORT_MIN;
	else axis = 0;
}

void GamepadClient::setDpadFromAxis(WORD& wButtons, const WORD padDirPositive, const WORD padDirNegative, const SHORT& axis)
{
	Bitwise::setValue(wButtons, padDirPositive, axis > GAMEPAD_DEADZONE);
	Bitwise::setValue(wButtons, padDirNegative, axis < -GAMEPAD_DEADZONE);
}

void GamepadClient::fetchDpadFromAxisX(XINPUT_STATE& state)
{
	setDpadFromAxis(state.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, XUSB_GAMEPAD_DPAD_LEFT, state.Gamepad.sThumbLX);
}

void GamepadClient::fetchDpadFromAxisY(XINPUT_STATE& state)
{
	setDpadFromAxis(state.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_UP, XUSB_GAMEPAD_DPAD_DOWN, state.Gamepad.sThumbLY);
}

void GamepadClient::fecthAxisFromDpadX(XINPUT_STATE& state)
{
	setAxisFromSign(state.Gamepad.sThumbLX, getDpadSignX(state.Gamepad.wButtons));
}

void GamepadClient::fecthAxisFromDpadY(XINPUT_STATE& state)
{
	setAxisFromSign(state.Gamepad.sThumbLY, getDpadSignY(state.Gamepad.wButtons));
}

XINPUT_STATE GamepadClient::dpadToStick(XINPUT_STATE state)
{
	XINPUT_STATE result = state;

	setAxisFromSign(result.Gamepad.sThumbLX, getDpadSignX(state.Gamepad.wButtons));
	setAxisFromSign(result.Gamepad.sThumbLY, getDpadSignY(state.Gamepad.wButtons));

	return result;
}

XINPUT_STATE GamepadClient::stickToDpad(XINPUT_STATE state)
{
	XINPUT_STATE result = state;

	setDpadFromAxis(result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_UP, XUSB_GAMEPAD_DPAD_DOWN, state.Gamepad.sThumbLY);
	setDpadFromAxis(result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, XUSB_GAMEPAD_DPAD_LEFT, state.Gamepad.sThumbLX);

	return result;
}


bool GamepadClient::isDpadEqual(XINPUT_STATE before, XINPUT_STATE now)
{
	const static WORD DPAD_MASK = XUSB_GAMEPAD_DPAD_UP | XUSB_GAMEPAD_DPAD_DOWN | XUSB_GAMEPAD_DPAD_LEFT | XUSB_GAMEPAD_DPAD_RIGHT;

	WORD dBefore = before.Gamepad.wButtons, dNow = now.Gamepad.wButtons;
	dBefore &= DPAD_MASK;
	dNow &= DPAD_MASK;

	return dBefore == dNow;
}

bool GamepadClient::isStickAsDpadEqual(XINPUT_STATE before, XINPUT_STATE now)
{
	const static WORD DPAD_MASK = XUSB_GAMEPAD_DPAD_UP | XUSB_GAMEPAD_DPAD_DOWN | XUSB_GAMEPAD_DPAD_LEFT | XUSB_GAMEPAD_DPAD_RIGHT;

	XINPUT_STATE dBefore = stickToDpad(before);
	XINPUT_STATE dNow = stickToDpad(now);

	return isDpadEqual(dBefore, dNow);
}