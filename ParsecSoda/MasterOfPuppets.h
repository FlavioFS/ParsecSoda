#pragma once

#include "GamepadClient.h"
#include "SDLGamepadReader.h"
#include "XInputReader.h"

class MasterOfPuppets
{
public:
	MasterOfPuppets();
	void init(GamepadClient& gamepadClient);
	
	void fetchSDLGamepads();
	void mapInputs();
	void setMasterIndex(int index);
	int getMasterIndex();

	vector<SDLGamepad>& getSDLGamepads();
	vector<GamepadState>& getXInputGamepads();

	bool isSDLEngine;

private:
	void mapFromSDL();
	void mapFromXInput();
	int masterIndex;

	GamepadClient* _gamepadClient;
	SDLGamepadReader _sdlReader;
	XInputReader _xinputReader;
};

