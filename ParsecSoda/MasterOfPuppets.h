#pragma once

#include "GamepadClient.h"
#include "SDLGamepadReader.h"
#include "XInputReader.h"
#include "Stopwatch.h"
#include <thread>
#include <mutex>

class MasterOfPuppets
{
public:
	MasterOfPuppets();
	void init(GamepadClient& gamepadClient);
	
	void start();
	void stop();

	void fetchSDLGamepads();
	void mapInputs();
	void setMasterIndex(int index);
	int getMasterIndex();
	void setVirtualMaster(bool enabled);
	bool getVirtualMaster();
	void setVirtualMasterState(XINPUT_GAMEPAD vpad);

	vector<SDLGamepad>& getSDLGamepads();
	vector<GamepadState>& getXInputGamepads();

	mutex inputMutex;
	Stopwatch stopwatch;
	bool isSDLEngine;

private:
	void internalFetchSDLGamepads();
	void mapFromVirtualMaster();
	void mapFromSDL();
	void mapFromXInput();
	bool isRunning = false;
	bool _isVirtualPad = false;
	
	int masterIndex;

	GamepadClient* _gamepadClient;
	SDLGamepadReader _sdlReader;
	XInputReader _xinputReader;
	XINPUT_STATE _vMasterState;

	thread _thread;
};

