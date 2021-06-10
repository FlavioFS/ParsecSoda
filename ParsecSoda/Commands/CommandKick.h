#pragma once

#include "ACommandSearchUser.h"
#include <iostream>
#include "parsec.h"


class CommandKick : public ACommandSearchUser
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::KICK; }

	CommandKick(const char* msg, Guest& sender, Parsec* parsec, GuestList &guests)
		:ACommandSearchUser(msg, internalPrefixes(), guests), _sender(sender), _parsec(parsec)
	{}

	bool run() override
	{
		ACommandSearchUser::run();
		
		switch (_searchResult)
		{
		case SEARCH_USER_RESULT::NOT_FOUND:
			{
				_replyMessage = std::string() + "[ChatBot] | " + _sender.name + ", I cannot find the user you want to kick.\0";
			}
			break;

		case SEARCH_USER_RESULT::FOUND:
			if (_sender.userID == _targetGuest.userID)
			{
				_replyMessage = std::string() + "[ChatBot] | Thou shall not kick thyself, " + _sender.name + " ...\0";
			}
			else
			{
				_replyMessage = std::string() + "[ChatBot] | " + _targetGuest.name+ " was kicked by " + _sender.name + "!\0";
				ParsecHostKickGuest(_parsec, _targetGuest.id);
				return true;
			}
			break;
		
		case SEARCH_USER_RESULT::FAILED:
		default:
			_replyMessage = "[ChatBot] | Usage: !kick <username>\nExample: !kick melon\0";
			break;
		}

		return false;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!kick" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!kick " };
	}

	Guest& _sender;
	Parsec* _parsec;
};

