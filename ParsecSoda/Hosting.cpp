#include "Hosting.h"

using namespace std;

#if defined(_WIN32)
	#if !defined(BITS)
		#define BITS 64
	#endif
	#if (BITS == 64)
		#define SDK_PATH "./parsec.dll"
	#else
		#define SDK_PATH "./parsec32.dll"
	#endif
#endif

// ============================================================
// 
//  PUBLIC
// 
// ============================================================

Hosting::Hosting()
{
	_hostConfig = EMPTY_HOST_CONFIG;
	setHostConfig("Let's have fun!", "", 2, false, "melonsod");

	_sfxList.init("./sfx/custom/_sfx.json");

	const vector<int> admins{ 3888558 , 6711547 };
	_adminList = AdminList(admins);

	const vector<GuestData> banned
	{
		GuestData("Miamiheatfan34", 1227313),
		GuestData("chapiusk", 6949214)
	};
	_banList = BanList(banned);
	
	_parsec = nullptr;
}

void Hosting::applyHostConfig()
{
	if (isRunning())
	{
		ParsecHostSetConfig(_parsec, &_hostConfig, _parsecSession.sessionId.c_str());
	}
}

void Hosting::broadcastChatMessage(string message)
{
	vector<Guest> guests = _guestList.getGuests();
	vector<Guest>::iterator gi;

	for (gi = guests.begin(); gi != guests.end(); ++gi)
	{
		ParsecHostSendUserData(_parsec, (*gi).id, HOSTING_CHAT_MSG_ID, message.c_str());
	}
}

void Hosting::init()
{
	_parsecStatus = ParsecInit(NULL, NULL, (char *)SDK_PATH, &_parsec);
	_dx11.init();
	_gamepadClient.init();
	_gamepadClient.createMaximumGamepads();

	audioOut.fetchDevices();
	audioOut.setOutputDevice();		// TODO Fix leak in setOutputDevice
	vector<AudioOutDevice> audioOutDevices = audioOut.getDevices();
	audioOut.captureAudio();
	audioOut.volume = 0.3f;

	vector<AudioInDevice> devices = audioIn.listInputDevices();
	AudioInDevice device = audioIn.selectInputDevice("cable out");
	audioIn.init(device);
	audioIn.volume = 0.8f;

	// Data is mocked for now - arguments don't matter
	//_parsecSession.fetchSession(EMAIL, PASSWORD);
	_parsecSession.mockSession();	// Replace with fetchSession in final version
	//_parsecSession.fetchArcadeRoomList();

	_host.name = "Host";
	_host.status = Guest::Status::INVALID;
	if (isReady())
	{
		//_parsecSession.fetchAccountData(&_host);
	}

	_chatBot = new ChatBot(
		audioIn, audioOut, _banList, _dice, _dx11,
		_gamepadClient, _guestList, _parsec,
		_hostConfig, _parsecSession, _sfxList, _isRunning, _host
	);
}

void Hosting::release()
{
	stopHosting();
	while (_isRunning)
	{
		Sleep(5);
	}
	_dx11.clear();
	_gamepadClient.release();
}

bool Hosting::isReady()
{
	return _parsecStatus == PARSEC_OK;
}

bool Hosting::isRunning()
{
	return _isRunning;
}

bool& Hosting::isGamepadLock()
{
	return _gamepadClient.lock;
}

Guest& Hosting::getHost()
{
	return _host;
}

ParsecHostConfig& Hosting::getHostConfig()
{
	return _hostConfig;
}

vector<string>& Hosting::getMessageLog()
{
	return _chatLog.getMessageLog();
}

vector<string>& Hosting::getCommandLog()
{
	return _chatLog.getCommandLog();
}

vector<Guest>& Hosting::getGuestList()
{
	return _guestList.getGuests();
}

vector<Gamepad>& Hosting::getGamepads()
{
	return _gamepadClient.gamepads;
}

const char** Hosting::getGuestNames()
{
	return _guestList.guestNames;
}

void Hosting::toggleGamepadLock()
{
	_gamepadClient.lock = !_gamepadClient.lock;
}

void Hosting::setGameID(string gameID)
{
	try
	{
		strcpy_s(_hostConfig.gameID, gameID.c_str());
	}
	catch (const std::exception&) {}
}

void Hosting::setMaxGuests(uint8_t maxGuests)
{
	_hostConfig.maxGuests = maxGuests;
}

void Hosting::setHostConfig(string roomName, string gameId, uint8_t maxGuests, bool isPublicRoom)
{
	setRoomName(roomName);
	setGameID(gameId);
	setMaxGuests(maxGuests);
	setPublicRoom(isPublicRoom);
}

void Hosting::setHostConfig(string roomName, string gameId, uint8_t maxGuests, bool isPublicRoom, string secret)
{
	setRoomName(roomName);
	setGameID(gameId);
	setMaxGuests(maxGuests);
	setPublicRoom(isPublicRoom);
	setRoomSecret(secret);
}

void Hosting::setPublicRoom(bool isPublicRoom)
{
	_hostConfig.publicGame = isPublicRoom;
}

void Hosting::setRoomName(string roomName)
{
	try
	{
		strcpy_s(_hostConfig.name, roomName.c_str());
	}
	catch (const std::exception&) {}
}

void Hosting::setRoomSecret(string secret)
{
	try
	{
		strcpy_s(_hostConfig.secret, secret.c_str());
	}
	catch (const std::exception&) {}
}

void Hosting::startHosting()
{
	if (!_isRunning)
	{
		_isRunning = true;
		initAllModules();

		try
		{
			if (_parsec != nullptr)
			{
				_mediaThread = thread ( [this]() {liveStreamMedia(); } );
				_inputThread = thread ([this]() {pollInputs(); });
				_eventThread = thread ([this]() {pollEvents(); });
				_mainLoopControlThread = thread ([this]() {mainLoopControl(); });
			}
		}
		catch (const exception&)
		{}
	}

	bool debug = true;
}

void Hosting::stopHosting()
{
	_isRunning = false;
}

void Hosting::stripGamepad(int index)
{
	_gamepadClient.clearOwner(index);
}

void Hosting::setOwner(Gamepad& gamepad, Guest newOwner, int padId)
{
	bool found = _gamepadClient.findPreferences(newOwner.userID, [&](GamepadClient::GuestPreferences& prefs) {
		gamepad.setOwner(newOwner, padId, prefs.mirror);
	});

	if (!found)
	{
		gamepad.setOwner(newOwner, padId, false);
	}
}

void Hosting::handleMessage(const char* message, Guest& guest, bool& isAdmin, bool isHost)
{
	ACommand* command = _chatBot->identifyUserDataMessage(message, guest, isAdmin, isHost);
	command->run();

	// Non-blocked default message
	if (!isFilteredCommand(command))
	{
		CommandDefaultMessage defaultMessage(message, guest, _chatBot->getLastUserId(), isAdmin);
		defaultMessage.run();
		_chatBot->setLastUserId(guest.userID);

		if (!defaultMessage.replyMessage().empty())
		{
			_chatLog.logMessage(defaultMessage.replyMessage());
			broadcastChatMessage(defaultMessage.replyMessage());
			cout << endl << defaultMessage.replyMessage();
		}
	}

	// Chatbot's command reply
	if (!command->replyMessage().empty() && command->type() != COMMAND_TYPE::DEFAULT_MESSAGE)
	{
		_chatLog.logCommand(command->replyMessage());
		broadcastChatMessage(command->replyMessage());
		cout << endl << command->replyMessage();
		_chatBot->setLastUserId();
	}

	delete command;
}

void Hosting::sendHostMessage(const char* message)
{
	static bool isAdmin = true;
	handleMessage(message, _host, isAdmin, true);
}


// ============================================================
// 
//  PRIVATE
// 
// ============================================================
void Hosting::initAllModules()
{
	_dice.init();

	// Instance all gamepads at once
	_gamepadClient.connectAllGamepads();
	_gamepadClient.sortGamepads();
	//_gamepadClient.setLimit(3888558, 0);		// Remove myself
	//_gamepadClient.setLimit(6711547, 0);

	parsecArcadeStart();
}

void Hosting::liveStreamMedia()
{
	_mediaMutex.lock();
	_isMediaThreadRunning = true;

	using clock = chrono::system_clock;
	using milli = chrono::duration<double, milli>;
	const double FPS = 250.0;
	const double MS_PER_FRAME = 1000.0 / FPS;

	while (_isRunning)
	{
		const auto before = clock::now();
		_dx11.captureScreen(_parsec);

		audioIn.captureAudio();
		audioOut.captureAudio();
		if (audioIn.isReady() && audioOut.isReady())
		{
			vector<int16_t> mixBuffer = _audioMix.mix(audioIn.popBuffer(), audioOut.popBuffer());
			ParsecHostSubmitAudio(_parsec, PCM_FORMAT_INT16, audioOut.getFrequency(), mixBuffer.data(), mixBuffer.size() / 2);
		}

		const milli duration = clock::now() - before;

		if (duration.count() < MS_PER_FRAME)
		{
			Sleep(MS_PER_FRAME - duration.count());
		}
	}

	_isMediaThreadRunning = false;
	_mediaMutex.unlock();
	_mediaThread.detach();
}

void Hosting::mainLoopControl()
{
	do
	{
		Sleep(50);
	} while (!_isRunning);

	_isRunning = true;

	_mediaMutex.lock();
	_inputMutex.lock();
	_eventMutex.lock();

	ParsecHostStop(_parsec);
	_gamepadClient.disconnectAllGamepads();
	_isRunning = false;

	_mediaMutex.unlock();
	_inputMutex.unlock();
	_eventMutex.unlock();

	_mainLoopControlThread.detach();
}

void Hosting::pollEvents()
{
	_eventMutex.lock();
	_isEventThreadRunning = true;

	string chatBotReply;
	bool isAdmin = false;

	ParsecGuest* guests = nullptr;
	int guestCount = 0;

	ParsecHostEvent event;

	while (_isRunning)
	{
		if (ParsecHostPollEvents(_parsec, 30, &event)) {
			ParsecGuest parsecGuest = event.guestStateChange.guest;
			ParsecGuestState state = parsecGuest.state;
			Guest guest = Guest(parsecGuest.name, parsecGuest.userID, parsecGuest.id);
			isAdmin = _adminList.isAdmin(guest.userID);
			guestCount = ParsecHostGetGuests(_parsec, GUEST_CONNECTED, &guests);
			_guestList.setGuests(guests, guestCount);

			switch (event.type)
			{
			case HOST_EVENT_GUEST_STATE_CHANGE:
				onGuestStateChange(state, guest);
				break;

			case HOST_EVENT_USER_DATA:
				char* msg = (char*)ParsecGetBuffer(_parsec, event.userData.key);

				if (event.userData.id == PARSEC_APP_CHAT_MSG)
				{
					handleMessage(msg, guest, isAdmin);
				}

				ParsecFree(_parsec, msg);
				break;
			}
		}
	}

	ParsecFree(_parsec, guests);
	_isEventThreadRunning = false;
	_eventMutex.unlock();
	_eventThread.detach();
}

void Hosting::pollInputs()
{
	_inputMutex.lock();
	_isInputThreadRunning = true;

	ParsecGuest inputGuest;
	ParsecMessage inputGuestMsg;

	while (_isRunning)
	{
		if (ParsecHostPollInput(_parsec, 4, &inputGuest, &inputGuestMsg))
		{
			if (!_gamepadClient.lock)
			{
				_gamepadClient.sendMessage(inputGuest, inputGuestMsg);
			}
		}
	}

	_isInputThreadRunning = false;
	_inputMutex.unlock();
	_inputThread.detach();
}

bool Hosting::parsecArcadeStart()
{
	if (isReady()) {
		//ParsecSetLogCallback(logCallback, NULL);
		ParsecStatus status = ParsecHostStart(_parsec, HOST_GAME, &_hostConfig, _parsecSession.sessionId.c_str());
		return status == PARSEC_OK;
	}
	return false;
}

bool Hosting::isFilteredCommand(ACommand* command)
{
	static vector<COMMAND_TYPE> filteredCommands{ COMMAND_TYPE::IP };

	COMMAND_TYPE type;
	for (vector<COMMAND_TYPE>::iterator it = filteredCommands.begin(); it != filteredCommands.end(); ++it)
	{
		type = command->type();
		if (command->type() == *it)
		{
			return true;
		}
	}

	return false;
}

void Hosting::onGuestStateChange(ParsecGuestState& state, Guest& guest)
{
	static string logMessage;

	if ((state == GUEST_CONNECTED || state == GUEST_CONNECTING) && _banList.isBanned(guest.userID))
	{
		ParsecHostKickGuest(_parsec, guest.id);
		logMessage = _chatBot->formatBannedGuestMessage(guest);
		broadcastChatMessage(logMessage);
	}
	else if (state == GUEST_CONNECTED || state == GUEST_DISCONNECTED)
	{
		static string guestMsg;
		guestMsg.clear();
		guestMsg = string(guest.name);

		logMessage = _chatBot->formatGuestConnection(guest, state);
		broadcastChatMessage(logMessage);
		_chatLog.logCommand(logMessage);

		if (state != GUEST_CONNECTED)
		{
			int droppedPads = 0;
			CommandFF command(guest, _gamepadClient);
			command.run();
			if (droppedPads > 0)
			{
				logMessage = command.replyMessage();
				broadcastChatMessage(logMessage);
				_chatLog.logCommand(logMessage);
			}
		}
	}
}



// ============================================================
// 
//  Delete queue
// 
// ============================================================

//void logDataMessage(const char* msg, ParsecHostEvent event)
//{
//	string msgStr = msg;
//	cout
//		<< endl
//		<< "DATA"
//		<< " | userid: " << event.userData.guest.userID
//		<< " | guestid: " << event.userData.guest.id
//		<< " | msgid: " << event.userData.id
//		<< " | key: " << event.userData.key
//		<< " | msg: " << msg;
//}

//static void logCallback(ParsecLogLevel level, const char *msg, void *opaque)
//{
//	opaque;
//	if (level != LOG_DEBUG)
//	{
//		cout << "[I] " << msg << endl;
//	}
//	printf("[%s] %s\n", level == LOG_DEBUG ? "D" : "I", msg);
//}