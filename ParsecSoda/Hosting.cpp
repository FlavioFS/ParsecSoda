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
	vector<Guest>::iterator i;

	for (i = guests.begin(); i != guests.end(); ++i)
	{
		ParsecHostSendUserData(_parsec, (*i).getID(), HOSTING_CHAT_MSG_ID, message.c_str());
	}
}

void Hosting::init()
{
	_parsecStatus = ParsecInit(PARSEC_VER, NULL, NULL, &_parsec);
	_dx11.init();
	_gamepadClient.init();
	_gamepadClient.createMaximumGamepads();
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
				_mainLoopControlThread = thread ([this]() {mainLoopControl(); });
				_mediaThread = thread ([this]() {liveStreamMedia(); });
				_inputThread = thread ([this]() {pollInputs(); });
				_eventThread = thread ([this]() {pollEvents(); });

				//inputThread.join();
				//eventsThread.join();
				//mediaThread.join();

				//_gamepadClient.release();
				//ParsecHostStop(_parsec);
				//ParsecDestroy(_parsec);
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
}

void Hosting::mainLoopControl()
{
	_isRunning = true;
	while (_isRunning)
	{
		Sleep(100);
	}

	while (_isMediaThreadRunning || _isInputThreadRunning || _isEventThreadRunning)
	{
		Sleep(100);
	}

	ParsecHostStop(_parsec);

	_mediaThread.detach();
	_inputThread.detach();
	_eventThread.detach();
	_isRunning = false;

	_gamepadClient.disconnectAllGamepads();
	_mainLoopControlThread.detach();
}

void Hosting::pollEvents()
{
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
			ParsecGuest guest = event.guestStateChange.guest;

			switch (event.type)
			{
			case HOST_EVENT_GUEST_STATE_CHANGE:
				guestCount = ParsecHostGetGuests(_parsec, GUEST_CONNECTED, &guests);
				_guestList.setGuests(guests, guestCount);

				if ((guest.state == GUEST_CONNECTED || guest.state == GUEST_CONNECTING) && _banList.isBanned(guest.userID))
				{
					ParsecHostKickGuest(_parsec, guest.id);
					broadcastChatMessage(_chatBot.formatBannedGuestMessage(guest));
				}
				else if (guest.state == GUEST_CONNECTED || guest.state == GUEST_DISCONNECTED)
				{
					guestMsg.clear();
					guestMsg = string(event.guestStateChange.guest.name);

					isAdmin = _adminList.isAdmin(event.userData.guest.userID);
					broadcastChatMessage(_chatBot.formatGuestConnection(event.guestStateChange.guest, isAdmin));

					if (guest.state == GUEST_CONNECTED)
					{
						printf("%s (#%d)    >> joined\n", guest.name, guest.userID);
					}
					else
					{
						printf("%s (#%d)    quit >>\n", guest.name, guest.userID);
						int droppedPads = 0;
						CommandFF command;
						command.run(guest, &_gamepadClient, &droppedPads);
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
					dataGuest = event.userData.guest;
					guestCount = ParsecHostGetGuests(_parsec, GUEST_CONNECTED, &guests);
					isAdmin = _adminList.isAdmin(dataGuest.userID);
					ACommand* command = _chatBot.identifyUserDataMessage(msg);
					COMMAND_TYPE type = command->type();


					// =================================
					//  Pleb commands
					// =================================
					if (!isAdmin)
					{
						switch (command->type())
						{
						case COMMAND_TYPE::AFK:
							((CommandAFK*)command)->run(guests, guestCount, &_gamepadClient);
							break;
						case COMMAND_TYPE::BONK:
							((CommandBonk*)command)->run(msg, dataGuest, guests, guestCount, &_dice);
							break;
						case COMMAND_TYPE::COMMANDS:
							((CommandListCommands*)command)->run(isAdmin);
							break;
						case COMMAND_TYPE::FF:
							((CommandFF*)command)->run(dataGuest, &_gamepadClient);
							break;
						case COMMAND_TYPE::IP:
							((CommandIpFilter*)command)->run(_parsec, dataGuest, &_banList);
							break;
						case COMMAND_TYPE::MIRROR:
							((CommandMirror*)command)->run(dataGuest, &_gamepadClient);
							break;
						case COMMAND_TYPE::PADS:
							((CommandPads*)command)->run(&_gamepadClient);
							break;
						case COMMAND_TYPE::SWAP:
							((CommandSwap*)command)->run(msg, dataGuest, &_gamepadClient);
							break;
						default:
							break;
						}
					}

					// =================================
					//  Admin commands
					// =================================
					else
					{
						switch (command->type())
						{
						case COMMAND_TYPE::AFK:
							((CommandAFK*)command)->run(guests, guestCount, &_gamepadClient);
							break;
						case COMMAND_TYPE::BAN:
							((CommandBan*)command)->run(msg, _parsec, dataGuest, guests, guestCount, &_banList);
							break;
						case COMMAND_TYPE::BONK:
							((CommandBonk*)command)->run(msg, dataGuest, guests, guestCount, &_dice);
							break;
						case COMMAND_TYPE::COMMANDS:
							((CommandListCommands*)command)->run(isAdmin);
							break;
						case COMMAND_TYPE::DC:
							((CommandDC*)command)->run(msg, &_gamepadClient);
							break;
						case COMMAND_TYPE::FF:
							((CommandFF*)command)->run(dataGuest, &_gamepadClient);
							break;
						case COMMAND_TYPE::GAMEID:
							((CommandGameId*)command)->run(msg, &_hostConfig);
							break;
						case COMMAND_TYPE::GUESTS:
							((CommandGuests*)command)->run(msg, &_hostConfig);
							break;
						case COMMAND_TYPE::IP:
							((CommandIpFilter*)command)->run(_parsec, dataGuest, &_banList);
							break;
						case COMMAND_TYPE::KICK:
							((CommandKick*)command)->run(msg, _parsec, dataGuest, guests, guestCount);
							break;
						case COMMAND_TYPE::LIMIT:
							((CommandLimit*)command)->run(msg, guests, guestCount, &_gamepadClient);
							break;
						case COMMAND_TYPE::MIC:
							((CommandMic*)command)->run(msg, &_audioMix);
							break;
						case COMMAND_TYPE::MIRROR:
							((CommandMirror*)command)->run(dataGuest, &_gamepadClient);
							break;
						case COMMAND_TYPE::NAME:
							((CommandName*)command)->run(msg, &_hostConfig);
							break;
						case COMMAND_TYPE::PADS:
							((CommandPads*)command)->run(&_gamepadClient);
							break;
						case COMMAND_TYPE::PRIVATE:
							((CommandPrivate*)command)->run(&_hostConfig);
							break;
						case COMMAND_TYPE::PUBLIC:
							((CommandPublic*)command)->run(&_hostConfig);
							break;
						case COMMAND_TYPE::SWAP:
							((CommandSwap*)command)->run(msg, dataGuest, &_gamepadClient);
							break;
						case COMMAND_TYPE::QUIT:
							((CommandQuit*)command)->run(&_isRunning);
							break;
						case COMMAND_TYPE::SETCONFIG:
							((CommandSetConfig*)command)->run(_parsec, &_hostConfig, _parsecSession.sessionId.c_str());
							break;
						case COMMAND_TYPE::SPEAKERS:
							((CommandSpeakers*)command)->run(msg, &_audioMix);
							break;
						case COMMAND_TYPE::UNBAN:
							((CommandUnban*)command)->run(msg, dataGuest, &_banList);
							break;
						case COMMAND_TYPE::VIDEOFIX:
							((CommandVideoFix*)command)->run(&_dx11);
							break;
							//case COMMAND_TYPE::GIVE:
							//	break;
							//case COMMAND_TYPE::TAKE:
							//	break;
						default:
							break;
						}
					}

					// Blocked messages
					if (!isFilteredCommand(command))
					{
						CommandDefaultMessage defaultMessage;
						defaultMessage.run(msg, dataGuest, _chatBot.getLastUserId(), isAdmin);
						_chatBot.setLastUserId(dataGuest.userID);

						if (!defaultMessage.replyMessage().empty())
						{
							broadcastChatMessage(defaultMessage.replyMessage());
							cout << endl << defaultMessage.replyMessage();
						}
					}

					if (!command->replyMessage().empty())
					{
						broadcastChatMessage(command->replyMessage());
						cout << endl << command->replyMessage();
						_chatBot.setLastUserId();
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
}

void Hosting::pollInputs()
{
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





//void broadcastChatMessage(Parsec *ps, ParsecGuest *guests, int guestCount, string message)
//{
//	ParsecGuest it;
//	for (int i = 0; i < guestCount; i++)
//	{
//		it = guests[i];
//		ParsecHostSendUserData(ps, it.id, PARSEC_APP_CHAT_MSG, message.c_str());
//	}
//}

//void clearGuests(Parsec *ps, int *guestCount)
//{
//	if (*guestCount > 0)
//	{
//		ParsecFree(ps);
//		*guestCount = 0;
//	}
//}

// ============================================================
//
//  Enter the THREAD zone
//
// ============================================================
//Dice dice;
//GamepadClient padClient;
//bool isRunning = true;
//ParsecGuest* guests = NULL;
//ParsecSession parsecSession;
//AdminList adminList(admins);
//BanList banList(banned);
//ChatBot chatBot;
//Parsec* ps;
//DX11 dx11;
//AudioOut audioOut;
//AudioIn audioIn;
//AudioMix audioMix(0.8, 0.3);
//ParsecHostConfig hostConfig;

// ============================================================