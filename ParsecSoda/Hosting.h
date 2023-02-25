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
#include "TierList.h"
#include "ChatBot.h"
#include "ChatLog.h"
#include "Stringer.h"
#include "AudioIn.h"
#include "AudioOut.h"
#include "AudioMix.h"
#include "GamepadClient.h"
#include "BanList.h"
#include "GuestList.h"
#include "SFXList.h"
#include "MetadataCache.h"
#include "CompilerDirectives.h"
#include "Stopwatch.h"
#include "MasterOfPuppets.h"
#include "GuestMetricsHistory.h"
#include "HotseatManager.h"

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
	void fetchAccountData(bool sync = false);
	ParsecHostConfig& getHostConfig();
	DX11& getDX11();
	ChatBot* getChatBot();
	ChatLog& getChatLog();
	GuestList& getGuestList();
	GuestDataList& getGuestHistory();
	GuestMetricsHistory& getGuestMetricsHistory();
	BanList& getBanList();
	vector<AGamepad*>& getGamepads();
	GamepadClient& getGamepadClient();
	MasterOfPuppets& getMasterOfPuppets();
	HotseatManager& getHotseatManager();
	SFXList& getSFXList();
	void toggleGamepadLock();
	void setGameID(string gameID);
	void setMaxGuests(uint8_t maxGuests);
	void setHostConfig(string roomName, string gameId, uint8_t roomSlots, bool isPublicRoom);
	void setHostConfig(string roomName, string gameId, uint8_t roomSlots, bool isPublicRoom, string secret);
	void setHostVideoConfig(uint32_t fps, uint32_t bandwidth);
	void setPublicRoom(bool isPublicRoom);
	void setRoomName(string roomName);
	void setRoomSecret(string secret);
	void startHosting();
	void stopHosting();
	void stripGamepad(int index);

	void handleMessage(const char* message, Guest& guest, bool isHost = false, bool isHidden = false);
	void sendHostMessage(const char* message, bool isHidden = false);

	AudioIn audioIn;
	AudioOut audioOut;

private:
	void initAllModules();
	void liveStreamMedia();
	void mainLoopControl();
	void pollEvents();
	void pollInputs();
	void pollMetrics();
	void pollHotseats();
	bool parsecArcadeStart();
	bool isFilteredCommand(ACommand* command);
	void onGuestStateChange(ParsecGuestState& state, Guest& guest);

	// Attributes
	AudioMix _audioMix;
	DX11 _dx11;
	BanList _banList;
	GuestList _guestList;
	GuestDataList _guestHistory;
	GuestMetricsHistory _guestMetricsHistory;

	ChatBot *_chatBot;
	ChatLog _chatLog;
	GamepadClient _gamepadClient;
	MasterOfPuppets _masterOfPuppets;
	HotseatManager _hotseatManager;
	
	ParsecDSO* _parsec;
	ParsecHostConfig _hostConfig;
	ParsecSession _parsecSession;
	ParsecStatus _parsecStatus;
	Guest _host;
	SFXList _sfxList;
	TierList _tierList;

	bool _isRunning = false;
	bool _isMediaThreadRunning = false;
	bool _isInputThreadRunning = false;
	bool _isMetricsThreadRunning = false;
	bool _isEventThreadRunning = false;
	bool _isHotseatsThreadRunning = false;

	Stopwatch _mediaClock;

	thread _mainLoopControlThread;
	thread _mediaThread;
	thread _inputThread;
	thread _eventThread;
	thread _metricsThread;
	thread _hotseatsThread;
	thread _createGamepadsThread;
	thread _connectGamepadsThread;

	mutex _mediaMutex;
	mutex _inputMutex;
	mutex _eventMutex;
	mutex _metricsMutex;
	mutex _hotSeatsMutex;
};