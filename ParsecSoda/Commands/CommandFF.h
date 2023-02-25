#pragma once

#include <iostream>
#include <sstream>
#include "ACommand.h"
#include "../GamepadClient.h"
#include "../HotseatManager.h"
#include "../Guest.h"

class CommandFF : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::FF; }

	CommandFF(Guest &sender, GamepadClient &gamepadClient, HotseatManager& hotseatManager)
		: _sender(sender), _gamepadClient(gamepadClient), _hotseatManager(hotseatManager), _droppedPadCount(0)
	{}

	bool run() override
	{
		if (_hotseatManager.isEnabled())
		{
			return runHotseats();
		}
		else
		{
			return runDefault();
		}
	}

	bool runDefault()
	{
		_droppedPadCount = _gamepadClient.onQuit(_sender);

		std::ostringstream reply;
		if (_droppedPadCount > 1)
		{
			reply << "[ChatBot]\t" << _sender.name << " has dropped " << _droppedPadCount << " gamepads!\0";
		}
		else if (_droppedPadCount > 0)
		{
			reply << "[ChatBot]\t" << _sender.name << " has dropped " << _droppedPadCount << " gamepad!\0";
		}
		else
		{
			reply << "[ChatBot]\t" << _sender.name << " has no gamepads to drop.\0";
		}

		_replyMessage = reply.str();
		reply.clear();

		return true;
	}

	bool runHotseats()
	{
		_replyMessage.clear();

		if (_hotseatManager.spectateGuestID(_sender.userID))
		{
			_replyMessage = "[ChatBot]\t" + _sender.name + " is now only watching.\0";
			return true;
		}

		return false;
	}

	int droppedPadCount()
	{
		return _droppedPadCount;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!ff", "!drop", "!quit", "!spectate" };
	}

protected:
	Guest& _sender;
	GamepadClient &_gamepadClient;
	HotseatManager& _hotseatManager;
	int _droppedPadCount;
};