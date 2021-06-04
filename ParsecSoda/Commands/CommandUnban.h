#pragma once

#include "ACommandStringArg.h"
#include <iostream>
#include <sstream>
#include "parsec.h"
#include "../BanList.h"

class CommandUnban : public ACommandStringArg
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::UNBAN; }

	void run(const char* msg, ParsecGuest sender, BanList * banList)
	{
		std::string bannedGuestName = "";
		if ( !ACommandStringArg::run(msg, "/unban ", &bannedGuestName))
		{
			_replyMessage = "[ChatBot] | Usage: /unban <username>\nExample: /unban melon\0";
		}
		
		GuestData unbannedGuest;
		if (banList->unban(bannedGuestName, &unbannedGuest))
		{
			std::ostringstream reply;
			reply
				<< "[ChatBot] | " << sender.name << " has revoked the ban from a guest: "
				<< unbannedGuest.name << "\t(#" << unbannedGuest.userId << ")\0";
			_replyMessage = reply.str();
		}
		else
		{
			_replyMessage = std::string() + "[ChatBot] | " + sender.name + ", I cannot find the user you want to unban.\0";
		}
	}
};

