#pragma once

#include "ACommandSearchUser.h"
#include <iostream>
#include "parsec-dso.h"
#include "../BanList.h"

using namespace std;

class CommandBan : public ACommandSearchUser
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::BAN; }

	CommandBan(const char* msg, Guest& sender, ParsecDSO* parsec, GuestList &guests, BanList &banList)
		: ACommandSearchUser(msg, internalPrefixes(), guests), _sender(sender), _parsec(parsec), _ban(banList)
	{
	}

	bool run() override
	{
		ACommandSearchUser::run();
		
		bool rv = false;

		switch (_searchResult)
		{
		case SEARCH_USER_RESULT::NOT_FOUND:
			_replyMessage = string() + "[ChatBot] | " + _sender.name + ", I cannot find the user you want to ban.\0";
			break;

		case SEARCH_USER_RESULT::FOUND:
			if (_sender.userID == _targetGuest.userID)
				_replyMessage = string() + "[ChatBot] | Thou shall not ban thyself, " + _sender.name + " ...\0";
			else
			{
				_replyMessage = string() + "[ChatBot] | " + _targetGuest.name + " was banned by " + _sender.name + "!\0";
				
				if (!_targetGuest.isHost)
				{
					_ban.ban(GuestData(_targetGuest.name, _targetGuest.userID));
					ParsecHostKickGuest(_parsec, _targetGuest.id);
				}
				
				rv = true;
			}
			break;
		
		case SEARCH_USER_RESULT::FAILED:
		default:
			_replyMessage = "[ChatBot] | Usage: !ban <username>\nExample: !ban melon\0";
			break;
		}

		return rv;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!ban" };
	}

private:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!ban " };
	}

	ParsecDSO* _parsec;
	Guest& _sender;
	BanList _ban;
};