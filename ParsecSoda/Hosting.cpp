#include "Hosting.h"

using namespace std;

const vector<int> admins { 3888558 , 6711547 };
const vector<GuestData> banned {};
vector<COMMAND_TYPE> filteredCommands { COMMAND_TYPE::IP };


void logDataMessage(const char* msg, ParsecHostEvent event)
{
	string msgStr = msg;
	cout
		<< endl
		<< "DATA"
		<< " | userid: " << event.userData.guest.userID
		<< " | guestid: " << event.userData.guest.id
		<< " | msgid: " << event.userData.id
		<< " | key: " << event.userData.key
		<< " | msg: " << msg;
}

bool isFilteredCommand(ACommand* command)
{
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



// ============================================================
// 
//  PUBLIC
// 
// ============================================================

static void logCallback(ParsecLogLevel level, const char *msg, void *opaque)
{
	opaque;
	if (level != LOG_DEBUG)
	{
		cout << "[I] " << msg << endl;
	}
	//printf("[%s] %s\n", level == LOG_DEBUG ? "D" : "I", msg);
}

Hosting::Hosting()
{
	_hostConfig = EMPTY_HOST_CONFIG;
	setHostConfig("Let's have fun!", "", 2, false, "melonsod");

	const vector<int> admins{ 3888558 , 6711547 };
	_adminList = AdminList(admins);
	
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
	_parsecStatus = ParsecInit(PARSEC_VER, NULL, NULL, &_parsec);
	_dx11.init();
	_gamepadClient.init();
	_gamepadClient.createMaximumGamepads();

	_chatBot = new ChatBot(_audioMix, _banList, _dice, _dx11, _gamepadClient, _guestList, _parsec, _hostConfig, _parsecSession, _isRunning);
}

bool Hosting::isReady()
{
	return _parsecStatus == PARSEC_OK;
}

bool Hosting::isRunning()
{
	return _isRunning;
}

ParsecHostConfig Hosting::getHostConfig()
{
	return _hostConfig;
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
				_mediaThread = thread ([this]() {liveStreamMedia(); });
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
	_gamepadClient.setLimit(3888558, 0);		// Remove myself
	_gamepadClient.setLimit(6711547, 0);

	// Data is mocked for now - arguments don't matter
	_parsecSession.mockSession();	// Replace with fetchSession in final version
	//parsecSession.fetchSession(EMAIL, PASSWORD);

	_audioOut.fetchDevices();
	_audioOut.setOutputDevice();		// TODO Fix leak in setOutputDevice
	vector<AudioOutDevice> audioOutDevices = _audioOut.getDevices();
	_audioOut.captureAudio();

	vector<AudioInDevice> devices = _audioIn.listInputDevices();
	AudioInDevice device = _audioIn.selectInputDevice("cable out");
	_audioIn.init(device);

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

		_audioIn.captureAudio();
		_audioOut.captureAudio();
		if (_audioIn.isReady() && _audioOut.isReady())
		{
			vector<int16_t> inBuffer = _audioIn.popBuffer();
			vector<int16_t> outBuffer = _audioOut.popBuffer();
			vector<int16_t> mixBuffer = _audioMix.mix(&inBuffer, &outBuffer);
			ParsecHostSubmitAudio(_parsec, PCM_FORMAT_INT16, _audioOut.getFrequency(), mixBuffer.data(), mixBuffer.size() / 2);
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

	ParsecGuest dataGuest;
	string guestMsg;
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
				if ((state == GUEST_CONNECTED || state == GUEST_CONNECTING) && _banList.isBanned(guest.userID))
				{
					ParsecHostKickGuest(_parsec, guest.id);
					broadcastChatMessage(_chatBot->formatBannedGuestMessage(guest));
				}
				else if (state == GUEST_CONNECTED || state == GUEST_DISCONNECTED)
				{
					guestMsg.clear();
					guestMsg = string(event.guestStateChange.guest.name);
					
					broadcastChatMessage(_chatBot->formatGuestConnection(guest, state, isAdmin));

					if (state == GUEST_CONNECTED)
					{
						printf("%s (#%d)    >> joined\n", guest.name, guest.userID);
					}
					else
					{
						printf("%s (#%d)    quit >>\n", guest.name, guest.userID);
						int droppedPads = 0;
						CommandFF command(guest, _gamepadClient);
						command.run();
						if (droppedPads > 0)
						{
							broadcastChatMessage(command.replyMessage());
						}
					}
				}
				break;

			case HOST_EVENT_USER_DATA:
				char* msg = (char*)ParsecGetBuffer(_parsec, event.userData.key);
				cout << msg << "\n";

				if (event.userData.id == PARSEC_APP_CHAT_MSG)
				{
					guestCount = ParsecHostGetGuests(_parsec, GUEST_CONNECTED, &guests);
					isAdmin = _adminList.isAdmin(guest.userID);
					ACommand* command = _chatBot->identifyUserDataMessage(msg, guest, isAdmin);
					command->run();

					// Non-blocked default message
					if (!isFilteredCommand(command))
					{
						CommandDefaultMessage defaultMessage (msg, guest, _chatBot->getLastUserId(), isAdmin);
						defaultMessage.run();
						_chatBot->setLastUserId(guest.userID);

						if (!defaultMessage.replyMessage().empty())
						{
							broadcastChatMessage(defaultMessage.replyMessage());
							cout << endl << defaultMessage.replyMessage();
						}
					}

					// Chatbot's command reply
					if (!command->replyMessage().empty() && command->type() != COMMAND_TYPE::DEFAULT_MESSAGE)
					{
						broadcastChatMessage(command->replyMessage());
						cout << endl << command->replyMessage();
						_chatBot->setLastUserId();
					}

					delete command;
				}

				ParsecFree(msg);
				break;
			}
		}
	}

	ParsecFree(guests);
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
			_gamepadClient.sendMessage(inputGuest, inputGuestMsg);
		}
	}

	_isInputThreadRunning = false;
	_inputMutex.unlock();
	_inputThread.detach();
}

bool Hosting::parsecArcadeStart()
{
	if (isReady()) {
		ParsecSetLogCallback(logCallback, NULL);
		ParsecStatus status = ParsecHostStart(_parsec, HOST_GAME, &_hostConfig, _parsecSession.sessionId.c_str());
		return status == PARSEC_OK;
	}
	return false;
}