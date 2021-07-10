#pragma once

#include "ACommandStringArg.h"
#include <iostream>
#include <sstream>
#include "parsec-dso.h"
#include "../BanList.h"
#include "../Guest.h"

class CommandUnban : public ACommandStringArg
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::UNBAN; }

	CommandUnban(const char* msg, Guest &sender, BanList &banList, GuestDataList& guestHistory)
		: ACommandStringArg(msg, internalPrefixes()), _sender(sender), _ban(banList), _guestHistory(guestHistory)
	{}

	bool run() override
	{
		if ( !ACommandStringArg::run())
		{
			_replyMessage = "[ChatBot] | Usage: !unban <username>\nExample: !unban melon\0";
			return false;
		}
		
		GuestData unbannedGuest;
		bool found = false;

		try
		{
			found = _ban.unban(stoul(_stringArg), [&unbannedGuest](GuestData& guest) {
				unbannedGuest = guest;
			});
		}
		catch (const std::exception&) { found = false; }

		if (!found)
		{
			found = _ban.unban(_stringArg, [&unbannedGuest](GuestData& guest) {
				unbannedGuest = guest;
			});
		}

		if (found)
		{
			std::ostringstream reply;
			reply
				<< "[ChatBot] | " << _sender.name << " has revoked a ban:\n"
				<< "\t\t" << unbannedGuest.name << "\t(#" << unbannedGuest.userID << ")\0";
			_replyMessage = reply.str();
			_guestHistory.add(unbannedGuest);
			return true;
		}
		else
		{
			_replyMessage = std::string() + "[ChatBot] | " + _sender.name + ", I cannot find the user you want to unban.\0";
			return false;
		}
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!unban" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!unban " };
	}
	string _msg;
	Guest& _sender;
	BanList& _ban;
	GuestDataList& _guestHistory;
};

