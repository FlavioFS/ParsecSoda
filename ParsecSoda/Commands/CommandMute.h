#pragma once

#include "ACommandSearchUser.h"
#include <iostream>
#include <Windows.h>
#include "parsec-dso.h"


class CommandMute : public ACommandSearchUser
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::MUTE; }

	CommandMute(const char* msg, Guest& sender, ParsecDSO* parsec, GuestList &guests, GuestDataList &mutedGuests)
		:ACommandSearchUser(msg, internalPrefixes(), guests), _sender(sender), _parsec(parsec), _mutedGuests(mutedGuests)
	{}

	bool run() override
	{
		ACommandSearchUser::run();
		
		switch (_searchResult)
		{
		case SEARCH_USER_RESULT::NOT_FOUND:
			{
				_replyMessage = std::string() + "[ChatBot]\t" + _sender.name + ", I cannot find the user you want to mute.\0";
			}
			break;

		case SEARCH_USER_RESULT::FOUND:
			if (_sender.userID == _targetGuest.userID)
			{
				_replyMessage = std::string() + "[ChatBot]\tThou shall not mute thyself, " + _sender.name + " ...\0";
			}
			else
			{
				bool isMuted = _mutedGuests.find(_targetGuest.userID);
				if (isMuted)
				{
					_mutedGuests.pop(_targetGuest.userID);
					_replyMessage = std::string() + "[ChatBot]\t" + _targetGuest.name+ " was unmuted by " + _sender.name + "!\0";
				}
				else
				{
					_mutedGuests.add(GuestData(_targetGuest.name, _targetGuest.userID));
					_replyMessage = std::string() + "[ChatBot]\t" + _targetGuest.name + " was muted by " + _sender.name + "!\0";
				}

				return true;
			}
			break;
		
		case SEARCH_USER_RESULT::FAILED:
		default:
			_replyMessage = "[ChatBot]\tUsage: !mute <username>\nExample: !mute melon\0";
			break;
		}

		return false;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!mute" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!mute " };
	}

	Guest& _sender;
	ParsecDSO* _parsec;
	GuestDataList& _mutedGuests;
};

