#pragma once

#include "ACommandSearchUserHistory.h"
#include <iostream>
#include <Windows.h>
#include <mmsystem.h>
#include "parsec-dso.h"
#include "../BanList.h"

using namespace std;

class CommandBan : public ACommandSearchUserHistory
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::BAN; }

	CommandBan(const char* msg, Guest& sender, ParsecDSO* parsec, GuestList &guests, GuestDataList &guestHistory, BanList &banList)
		: ACommandSearchUserHistory(msg, internalPrefixes(), guests, guestHistory), _sender(sender), _parsec(parsec), _ban(banList)
	{
	}

	bool run() override
	{
		ACommandSearchUserHistory::run();
		
		bool rv = false;

		switch (_searchResult)
		{
		case SEARCH_USER_HISTORY_RESULT::NOT_FOUND:
			_replyMessage = string() + "[ChatBot] | " + _sender.name + ", I cannot find the user you want to ban.\0";
			break;

		case SEARCH_USER_HISTORY_RESULT::ONLINE:
			handleGuest(GuestData(_onlineGuest.name, _onlineGuest.userID), true, _onlineGuest.id);
			_guestHistory.pop(_onlineGuest.userID);
			break;
		
		case SEARCH_USER_HISTORY_RESULT::OFFLINE:
			handleGuest(_offlineGuest, false);
			_guestHistory.pop(_offlineGuest.userID);
			break;

		case SEARCH_USER_HISTORY_RESULT::FAILED:
		default:
			_replyMessage = "[ChatBot] | Usage: !ban <username>\nExample: !ban melon\0";
			break;
		}

		return rv;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!ban", "!block" };
	}

private:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!ban ", "!block " };
	}

	ParsecDSO* _parsec;
	Guest& _sender;
	BanList& _ban;

	bool handleGuest(GuestData target, bool isOnline, uint32_t guestID = -1)
	{
		bool result = false;

		if (_sender.userID == target.userID)
			_replyMessage = string() + "[ChatBot] | Thou shall not ban thyself, " + _sender.name + " ...\0";
		else
		{
			_replyMessage = string() + "[ChatBot] | " + target.name + " was banned by " + _sender.name + "!\0";

			if (_ban.ban(target))
			{
				if (isOnline)
				{
					ParsecHostKickGuest(_parsec, guestID);
				}

				try
				{
					PlaySound(TEXT("./sfx/ban.wav"), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
				}
				catch (const std::exception&) {}

				result = true;
			}
		}

		return result;
	}
};