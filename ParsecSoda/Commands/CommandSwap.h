#pragma once

#include <iostream>
#include <sstream>
#include "ACommandStringArg.h"
#include "ACommandIntegerArg.h"
#include "../GamepadClient.h"

class CommandSwap : public ACommandIntegerArg
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::SWAP; }

	CommandSwap(const char* msg, Guest &sender, GamepadClient  &gamepadClient, HotseatManager &hotseatManager)
		: ACommandIntegerArg(msg, internalPrefixes()), _sender(sender), _gamepadClient(gamepadClient), _hotseatManager(hotseatManager)
	{}

	bool run() override
	{

		if (!ACommandIntegerArg::run())
		{
			bool recoverySuccess = false;

			const string msg = getMessage();
			if (!msg.empty())
			{
				ACommandStringArg stringArg(msg.c_str(), internalPrefixes());
				stringArg.run();
				if (Stringer::compareNoCase(stringArg.getArg(), "Any") == 0)
				{
					_intArg = 0;
					recoverySuccess = true;
				}
			}

			if (!recoverySuccess)
			{
				_replyMessage = "[ChatBot]\tUsage: !swap <integer in range [1, gamepadCount]>\nExample: !swap 4\0";
				return false;
			}
		}

		bool rv = false;
		std::ostringstream reply;

		if (_hotseatManager.isEnabled())
		{
			int seatIndex = _hotseatManager.setDesiredSeat(_sender.userID, _intArg - 1);

			reply << "[ChatBot]\tHotseats - Waiting for: ";
			if (seatIndex < 0) reply << "Any seat\0";
			else reply << "Seat " << seatIndex + 1 << "\0";
			_replyMessage = reply.str();

			return true;
		}

		GamepadClient::PICK_REQUEST result = _gamepadClient.pick(_sender, _intArg - 1);
		switch (result)
		{
		case GamepadClient::PICK_REQUEST::OK:
			reply
				<< "[ChatBot]\tGamepad " << _intArg << " was given to " << _sender.name << "\t(#" << _sender.userID << ")\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			rv = true;
			break;
		case GamepadClient::PICK_REQUEST::DISCONNECTED:
			reply
				<< "[ChatBot]\t" << _sender.name << ", gamepad " << _intArg << " is offline.\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::SAME_USER:
			reply
				<< "[ChatBot]\t" << _sender.name << ", you have that gamepad already.\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::TAKEN:
			reply
				<< "[ChatBot]\t" << _sender.name << ", the gamepad you tried to pick is already taken.\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::EMPTY_HANDS:
			reply
				<< "[ChatBot]\t" << _sender.name << ", you must be holding a gamepad to use !swap command.\n"
				<< "\t\tPress any face button (A, B, X, Y) to receive a random gamepad (if available).\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::LIMIT_BLOCK:
			reply
				<< "[ChatBot]\t" << _sender.name << ", your current gamepad limit is set to 0.\0";
			break;
		case GamepadClient::PICK_REQUEST::OUT_OF_RANGE:
			reply
				<< "[ChatBot]\t" << _sender.name << ", your gamepad index is wrong (valid range is [1, 4]).\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::PUPPET:
			reply
				<< "[ChatBot]\t" << _sender.name << ", puppet master is handling that gamepad.\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		default:
			break;
		}

		_replyMessage = reply.str();
		return rv;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!swap", "!pick", "!slot" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!swap ", "!pick ", "!slot " };
	}
	string _msg;
	Guest& _sender;
	GamepadClient& _gamepadClient;
	HotseatManager& _hotseatManager;
};

