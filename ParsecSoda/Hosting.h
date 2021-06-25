#pragma once

#define D3D_DEBUG_INFO

#include <iostream>
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <thread>
#include <chrono>
#include <mutex>
#include "parsec-dso.h"
#include "ParsecSession.h"
#include "DX11.h"
#include "matoya.h"
#include "AdminList.h"
#include "ChatBot.h"
#include "ChatLog.h"
#include "Stringer.h"
#include "Mock.h"
#include "AudioIn.h"
#include "AudioOut.h"
#include "AudioMix.h"
#include "GamepadClient.h"
#include "BanList.h"
#include "Dice.h"
#include "GuestList.h"
#include "SFXList.h"
#include "MetadataCache.h"

#define PARSEC_APP_CHAT_MSG 0
#define HOSTING_CHAT_MSG_ID 0

#define ROOM_NAME "Coding my own Parsec\nGamepad streaming\0"
#define ROOM_SECRET "melonsod"

using namespace std;

class Hosting
{
public:
	Hosting();
	void applyHostConfig();
	void broadcastChatMessage(string message);
	void init();
	void release();
	bool isReady();
	bool isRunning();
	bool& isGamepadLock();
	Guest& getHost();
	ParsecSession& getSession();
	ParsecHostConfig& getHostConfig();
	vector<string>& getMessageLog();
	vector<string>& getCommandLog();
	vector<Guest>& getGuestList();
	vector<Gamepad>& getGamepads();
	GamepadClient& getGamepadClient();
	const char** getGuestNames();
	void toggleGamepadLock();
	void setGameID(string gameID);
	void setMaxGuests(uint8_t maxGuests);
	void setHostConfig(string roomName, string gameId, uint8_t roomSlots, bool isPublicRoom);
	void setHostConfig(string roomName, string gameId, uint8_t roomSlots, bool isPublicRoom, string secret);
	void setPublicRoom(bool isPublicRoom);
	void setRoomName(string roomName);
	void setRoomSecret(string secret);
	void startHosting();
	void stopHosting();
	void stripGamepad(int index);
	void setOwner(Gamepad& gamepad, Guest newOwner, int padId);

	void handleMessage(const char* message, Guest& guest, bool& isAdmin, bool isHost = false);
	void sendHostMessage(const char* message);

	AudioIn audioIn;
	AudioOut audioOut;

private:
	void initAllModules();
	void liveStreamMedia();
	void mainLoopControl();
	void pollEvents();
	void pollInputs();
	bool parsecArcadeStart();
	bool isFilteredCommand(ACommand* command);
	void onGuestStateChange(ParsecGuestState& state, Guest& guest);

	// Attributes
	AudioMix _audioMix;
	DX11 _dx11;
	AdminList _adminList;
	BanList _banList;
	ChatBot *_chatBot;
	ChatLog _chatLog;
	Dice _dice;
	GamepadClient _gamepadClient;
	GuestList _guestList;
	
	ParsecDSO* _parsec;
	ParsecHostConfig _hostConfig;
	ParsecSession _parsecSession;
	ParsecStatus _parsecStatus;
	Guest _host;
	SFXList _sfxList;

	bool _isRunning = false;
	bool _isMediaThreadRunning = false;
	bool _isInputThreadRunning = false;
	bool _isEventThreadRunning = false;

	thread _mainLoopControlThread;
	thread _mediaThread;
	thread _inputThread;
	thread _eventThread;
	thread _createGamepadsThread;
	thread _connectGamepadsThread;

	mutex _mediaMutex;
	mutex _inputMutex;
	mutex _eventMutex;
};