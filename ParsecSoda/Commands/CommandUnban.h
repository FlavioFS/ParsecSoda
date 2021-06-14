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

	CommandUnban(const char* msg, Guest &sender, BanList &banList)
		: ACommandStringArg(msg, internalPrefixes()), _sender(sender), _ban(banList)
	{}

	bool run() override
	{
		if ( !ACommandStringArg::run())
		{
			_replyMessage = "[ChatBot] | Usage: !unban <username>\nExample: !unban melon\0";
			return false;
		}
		
		GuestData unbannedGuest;
		if (_ban.unban(_stringArg, &unbannedGuest))
		{
			std::ostringstream reply;
			reply
				<< "[ChatBot] | " << _sender.name << " has revoked a ban:\n"
				<< "\t\t" << unbannedGuest.name << "\t(#" << unbannedGuest.userId << ")\0";
			_replyMessage = reply.str();
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
};

