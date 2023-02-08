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
	MetadataCache::loadPreferences();
	setHostConfig(
		MetadataCache::preferences.roomName,
		MetadataCache::preferences.gameID,
		MetadataCache::preferences.guestCount,
		MetadataCache::preferences.publicRoom,
		MetadataCache::preferences.secret
	);
	setHostVideoConfig(MetadataCache::preferences.fps, MetadataCache::preferences.bandwidth);

	_sfxList.init("./sfx/custom/_sfx.json");
	
	_tierList.loadTiers();
	_tierList.saveTiers();

	vector<GuestData> banned = MetadataCache::loadBannedUsers();
	_banList.init(banned);

	_parsec = nullptr;

	SDL_Init(SDL_INIT_JOYSTICK);
	_masterOfPuppets.init(_gamepadClient);
	_masterOfPuppets.start();

	_hotseatManager.setTimer(MetadataCache::preferences.hotseatDurationSeconds);
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
	_guestList.getGuestsSafe([&](vector<Guest>& guests) {
		vector<Guest>::iterator gi;

		for (gi = guests.begin(); gi != guests.end(); ++gi)
		{
			ParsecHostSendUserData(_parsec, (*gi).id, HOSTING_CHAT_MSG_ID, message.c_str());
		}
	});
}

void Hosting::init()
{
	_parsecStatus = ParsecInit(NULL, NULL, (char *)SDK_PATH, &_parsec);
	_dx11.init();
	_gamepadClient.setParsec(_parsec);
	_gamepadClient.setHotseatManager(&_hotseatManager);
	_gamepadClient.init();

	MetadataCache::Preferences preferences = MetadataCache::loadPreferences();

	_createGamepadsThread = thread([&]() {
		_gamepadClient.createAllGamepads();
		_createGamepadsThread.detach();
	});

	audioOut.fetchDevices();
	vector<AudioSourceDevice> audioOutDevices = audioOut.getDevices();
	if (preferences.audioOutputDevice >= audioOutDevices.size()) {
		preferences.audioOutputDevice = 0;
	}
	audioOut.setDevice(preferences.audioOutputDevice);
	audioOut.captureAudio();
	audioOut.volume = 0.3f;
	audioOut.setFrequency(MetadataCache::preferences.speakersFrequency);

	audioIn.fetchDevices();
	vector<AudioSourceDevice> audioInputDevices = audioIn.getDevices();
	if (preferences.audioInputDevice >= audioInputDevices.size()) {
		preferences.audioInputDevice = 0;
	}
	audioIn.setDevice(preferences.audioInputDevice);
	audioIn.captureAudio();
	audioIn.volume = 0.8f;
	audioIn.setFrequency(MetadataCache::preferences.micFrequency);

	preferences.isValid = true;
	MetadataCache::savePreferences(preferences);
	_parsecSession.loadThumbnails();
	_parsecSession.loadSessionCache();

	fetchAccountData();

	_chatBot = new ChatBot(
		audioIn, audioOut, _banList, _dx11,
		_gamepadClient, _hotseatManager, _guestList, _guestHistory, _parsec,
		_hostConfig, _parsecSession, _sfxList, _tierList,
		_isRunning, _host
	);

	CommandBonk::init();
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
	_masterOfPuppets.stop();
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

ParsecSession& Hosting::getSession()
{
	return _parsecSession;
}

void Hosting::fetchAccountData(bool sync)
{
	_host.name = "Host";
	_host.status = Guest::Status::INVALID;
	if (isReady())
	{
		if (sync)
		{
			_parsecSession.fetchAccountDataSync(&_host);
		}
		else
		{
			_parsecSession.fetchAccountData(&_host);
		}
	}
}

ParsecHostConfig& Hosting::getHostConfig()
{
	return _hostConfig;
}

DX11& Hosting::getDX11()
{
	return _dx11;
}

ChatBot* Hosting::getChatBot()
{
	return _chatBot;
}

ChatLog& Hosting::getChatLog()
{
	return _chatLog;
}

GuestList& Hosting::getGuestList()
{
	return _guestList;
}

GuestDataList& Hosting::getGuestHistory()
{
	return _guestHistory;
}

GuestMetricsHistory& Hosting::getGuestMetricsHistory()
{
	return _guestMetricsHistory;
}

BanList& Hosting::getBanList()
{
	return _banList;
}

vector<AGamepad*>& Hosting::getGamepads()
{
	return _gamepadClient.gamepads;
}

GamepadClient& Hosting::getGamepadClient()
{
	return _gamepadClient;
}

MasterOfPuppets& Hosting::getMasterOfPuppets()
{
	return _masterOfPuppets;
}

HotseatManager& Hosting::getHotseatManager()
{
	return _hotseatManager;
}

void Hosting::toggleGamepadLock()
{
	_gamepadClient.toggleLock();
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

void Hosting::setHostVideoConfig(uint32_t fps, uint32_t bandwidth)
{
	_hostConfig.video->encoderFPS = fps;
	_hostConfig.video->encoderMaxBitrate = bandwidth;
	MetadataCache::preferences.fps = fps;
	MetadataCache::preferences.bandwidth = bandwidth;
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
				_metricsThread = thread([this]() { pollMetrics(); });
				_hotseatsThread = thread([this]() { pollHotseats(); });
				_mainLoopControlThread = thread ([this]() {mainLoopControl(); });
			}
		}
		catch (const exception&)
		{}
	}
}

void Hosting::stopHosting()
{
	_isRunning = false;
	_guestList.clearSafe();
}

void Hosting::stripGamepad(int index)
{
	_gamepadClient.clearOwner(index);
}

void Hosting::setOwner(AGamepad& gamepad, Guest newOwner, int padId)
{
	bool found = _gamepadClient.findPreferences(newOwner.userID, [&](GamepadClient::GuestPreferences& prefs) {
		gamepad.setOwner(newOwner, padId, prefs.mirror);
	});

	if (!found)
	{
		gamepad.setOwner(newOwner, padId, false);
	}
}

void Hosting::handleMessage(const char* message, Guest& guest, bool isHost, bool isHidden)
{
	ACommand* command = _chatBot->identifyUserDataMessage(message, guest, isHost);
	command->run();

	// Non-blocked default message
	if (!isFilteredCommand(command))
	{
		Tier tier = _tierList.getTier(guest.userID);

		CommandDefaultMessage defaultMessage(message, guest, _chatBot->getLastUserId(), tier);
		defaultMessage.run();
		_chatBot->setLastUserId(guest.userID);

		if (!defaultMessage.replyMessage().empty() && !isHidden)
		{
			broadcastChatMessage(defaultMessage.replyMessage());
			
			string adjustedMessage = defaultMessage.replyMessage();
			Stringer::replacePatternOnce(adjustedMessage, "%", "%%");
			_chatLog.logMessage(adjustedMessage);
		}
	}

	// Chatbot's command reply
	if (!command->replyMessage().empty() && command->type() != COMMAND_TYPE::DEFAULT_MESSAGE)
	{
		broadcastChatMessage(command->replyMessage());
		_chatLog.logCommand(command->replyMessage());
		_chatBot->setLastUserId();
	}

	delete command;
}

void Hosting::sendHostMessage(const char* message, bool isHidden)
{
	static bool isAdmin = true;
	handleMessage(message, _host, true, isHidden);
}


// ============================================================
// 
//  PRIVATE
// 
// ============================================================
void Hosting::initAllModules()
{
	// Instance all gamepads at once
	_connectGamepadsThread = thread([&]() {
		_gamepadClient.sortGamepads();
		_connectGamepadsThread.detach();
	});

	parsecArcadeStart();
}

void Hosting::liveStreamMedia()
{
	const std::lock_guard<mutex> lock(_mediaMutex);
	_isMediaThreadRunning = true;

	static uint32_t sleepTimeMs = 4;
	_mediaClock.setDuration(sleepTimeMs);
	_mediaClock.start();

	while (_isRunning)
	{
		_mediaClock.reset();

		_dx11.captureScreen(_parsec);

		/**
		 * This lambda is a workaround to a ParsecSDK bug.
		 * When using a virtual device (e.g.: VBAudio Cable),
		 * ParsecSDK crashes if an already started audio stream
		 * stops sending audio.
		 */
		static const auto submitSilence = [&]() {
			ParsecHostSubmitAudio(_parsec, PCM_FORMAT_INT16, audioOut.getFrequency(), nullptr, 0);
		};

		if (audioIn.isEnabled && audioOut.isEnabled)
		{
			audioIn.captureAudio();
			audioOut.captureAudio();
			if (audioIn.isReady() && audioOut.isReady())
			{
				vector<int16_t> mixBuffer = _audioMix.mix(audioIn.popBuffer(), audioOut.popBuffer());
				ParsecHostSubmitAudio(_parsec, PCM_FORMAT_INT16, audioOut.getFrequency(), mixBuffer.data(), (uint32_t)mixBuffer.size() / 2);
			}
			else { submitSilence(); }
		}
		else if (audioOut.isEnabled)
		{
			audioOut.captureAudio();
			if (audioOut.isReady())
			{
				vector<int16_t> buffer = audioOut.popBuffer();
				ParsecHostSubmitAudio(_parsec, PCM_FORMAT_INT16, audioOut.getFrequency(), buffer.data(), (uint32_t)buffer.size() / 2);
			}
			else { submitSilence(); }
		}
		else if (audioIn.isEnabled)
		{
			audioIn.captureAudio();
			if (audioIn.isReady())
			{
				vector<int16_t> buffer = audioIn.popBuffer();
				ParsecHostSubmitAudio(_parsec, PCM_FORMAT_INT16, audioIn.getFrequency(), buffer.data(), (uint32_t)buffer.size() / 2);
			}
			else { submitSilence(); }
		}
		else { submitSilence(); }

		sleepTimeMs = _mediaClock.getRemainingTime();
		if (sleepTimeMs > 0)
		{
			Sleep(sleepTimeMs);
		}
	}

	_isMediaThreadRunning = false;
	_mediaThread.detach();
}

void Hosting::mainLoopControl()
{
	do
	{
		Sleep(50);
	} while (!_isRunning);

	_isRunning = true;

	const std::lock_guard<mutex> lockMedia(_mediaMutex);
	const std::lock_guard<mutex> lockInput(_inputMutex);
	const std::lock_guard<mutex> lockEvent(_eventMutex);
	const std::lock_guard<mutex> lockMetrics(_metricsMutex);
	const std::lock_guard<mutex> lockHotseats(_hotSeatsMutex);
	 
	ParsecHostStop(_parsec);
	_isRunning = false;

	_mainLoopControlThread.detach();
}

void Hosting::pollEvents()
{
	const std::lock_guard<mutex> lock(_eventMutex);
	_isEventThreadRunning = true;

	string chatBotReply;

	ParsecGuest* guests = nullptr;
	int guestCount = 0;

	ParsecHostEvent event;

	while (_isRunning)
	{
		if (ParsecHostPollEvents(_parsec, 30, &event)) {
			ParsecGuest parsecGuest = event.guestStateChange.guest;
			ParsecGuestState state = parsecGuest.state;
			Guest guest = Guest(parsecGuest.name, parsecGuest.userID, parsecGuest.id);
			guestCount = ParsecHostGetGuests(_parsec, GUEST_CONNECTED, &guests);
			_guestList.setGuestsSafe(guests, guestCount);

			switch (event.type)
			{
			case HOST_EVENT_GUEST_STATE_CHANGE:
				onGuestStateChange(state, guest);
				break;

			case HOST_EVENT_USER_DATA:
				char* msg = (char*)ParsecGetBuffer(_parsec, event.userData.key);

				if (event.userData.id == PARSEC_APP_CHAT_MSG)
				{
					handleMessage(msg, guest);
				}

				ParsecFree(_parsec, msg);
				break;
			}
		}
	}

	ParsecFree(_parsec, guests);
	_isEventThreadRunning = false;
	_eventThread.detach();
}

void Hosting::pollInputs()
{
	const std::lock_guard<mutex> lock(_inputMutex);
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
	_inputThread.detach();
}

void Hosting::pollMetrics()
{
	const std::lock_guard<mutex> lock(_metricsMutex);
	_isMetricsThreadRunning = true;

	ParsecGuest* parsecGuests = nullptr;
	int guestCount = 0;

	Stopwatch stopwatch;
	stopwatch.setDurationSec(1);
	stopwatch.start();

	while (_isRunning)
	{
		guestCount = ParsecHostGetGuests(_parsec, GUEST_CONNECTED, &parsecGuests);

		_guestList.runThreadSafe([&]() {
			_guestMetricsHistory.getMutexLockContext([&]() {
				for (size_t i = 0; i < guestCount; i++)
				{
					_guestList.findUnsafe(parsecGuests[i].userID, [&](Guest* gi) {
						gi->metrics.updateMetrics(parsecGuests[i]);
						_guestMetricsHistory.pushUnsafe(*gi);
					});


				}
			});
		});

		if (MetadataCache::preferences.latencyLimiterEnabled)
		{
			static vector<string> hostMessageQueue;

			_guestList.getGuestsSafe([&](vector<Guest>& guests) {
				for (int i = guests.size()-1; i >= 0; --i)
				{
					if (guests[i].metrics.getLatency() > MetadataCache::preferences.maxLatencyMs)
					{
						hostMessageQueue.push_back((string("!kick ") + to_string(guests[i].userID)));
					}
				}
			});

			for (int i = hostMessageQueue.size()-1; i >= 0; --i)
			{
				sendHostMessage(hostMessageQueue[i].c_str(), true);
				hostMessageQueue.pop_back();
			}
		}


		if (stopwatch.isRunComplete())
		{
			stopwatch.reset();
		}
		else
		{
			Sleep(stopwatch.getRemainingTime());
		}
	}

	_isMetricsThreadRunning = false;
	_metricsThread.detach();
}

void Hosting::pollHotseats()
{
	const std::lock_guard<mutex> lock(_hotSeatsMutex);
	_isHotseatsThreadRunning = true;

	Stopwatch stopwatch;
	stopwatch.setDurationSec(1);
	stopwatch.start();

	const static HotseatManager::GuestToBoolAction isGuestOnline = [&](const GuestData& guest) { return _guestList.findUnsafe(guest.userID); };
	const static HotseatManager::GuestToBoolAction isGuestMultitap = [&](const GuestData& guest) {
		bool isMultitap = false;

		_gamepadClient.findPreferences(guest.userID, [&](GamepadClient::GuestPreferences& prefs) {
			isMultitap = (!prefs.ignoreDeviceID && prefs.padLimit > 1);
		});
		
		return isMultitap;
	};

	while (_isRunning)
	{
		if (_hotseatManager.isEnabled())
		{
			_hotseatManager.runThreadSafe([&]() {
				_guestList.runThreadSafe([&]() {

					_hotseatManager.updateAndRotate(_gamepadClient.gamepads, isGuestOnline, isGuestMultitap);
					_gamepadClient.loadFromHotseats(_guestList);

				});
			});
		}

		if (stopwatch.isRunComplete())
		{
			stopwatch.reset();
		}
		else
		{
			Sleep(stopwatch.getRemainingTime());
		}
	}

	_isHotseatsThreadRunning = false;
	_hotseatsThread.detach();
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

		if (state == GUEST_CONNECTED)
		{
			_guestHistory.add(GuestData(guest.name, guest.userID));
		}
		else
		{
			int droppedPads = 0;
			CommandFF command(guest, _gamepadClient, _hotseatManager);
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