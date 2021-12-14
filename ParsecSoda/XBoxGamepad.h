#pragma once

#include "AGamepad.h"

//#include <Windows.h>
//#include <Xinput.h>
//#include "ViGEm/Client.h"
//#include <vector>
//#include <iostream>
//#include <functional>
//#include "parsec-dso.h"
//#include "Bitwise.h"
//#include "KeyboardMaps.h"
//#include "GuestDevice.h"
//#include "Dice.h"

using namespace std;

//#define GAMEPAD_INDEX_ERROR -1
//#define GAMEPAD_DEADZONE 4096
//
//#define GAMEPAD_STICK_MIN -32768
//#define GAMEPAD_STICK_MAX 32767
//#define GAMEPAD_TRIGGER_MIN 0
//#define GAMEPAD_TRIGGER_MAX 255

class XBoxGamepad : public AGamepad
{
public:
	const AGamepad::Type type() override { return AGamepad::Type::XBOX; }

	XBoxGamepad();
	XBoxGamepad(ParsecDSO* parsec, PVIGEM_CLIENT client);

	//bool realloc();
	//bool disconnect();
	//void release();
	//bool isAttached();
	//void setIndex(ULONG index);
	//ULONG getIndex() const;
	bool refreshIndex() override;
	//XINPUT_STATE getState();
	//Keyboard& getKeyboard();

	// State mesages
	bool alloc() override;
	bool connect() override;
	void clearState() override;
	void setState(XINPUT_STATE state) override;
	void setStateSafe(XINPUT_STATE state) override;

	//void setOwner(Guest& guest, uint32_t deviceID, bool isKeyboard);
	//void copyOwner(XBoxGamepad pad);
	//void clearOwner();
	//const bool isOwned();
	//bool isConnected() const;
	//GuestDevice owner = GuestDevice();
	//bool isPuppet = false;

	//ParsecDSO * parsec;


//protected:
	//XINPUT_STATE fetchXInputState();
	//Keyboard _keyboard;
	//PVIGEM_CLIENT _client;
	//PVIGEM_TARGET _pad;
	//ULONG _index = -1;
	//bool _isAlive = false;
	//bool _isConnected = false;

	//VOID CALLBACK notification(PVIGEM_CLIENT Client,PVIGEM_TARGET Target,UCHAR LargeMotor,UCHAR SmallMotor,UCHAR LedNumber, LPVOID UserData);

	//XINPUT_STATE _currentState;
};