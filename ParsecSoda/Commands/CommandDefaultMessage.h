#pragma once

#include <iostream>
#include <sstream>
#include "ACommand.h"
#include "../GuestData.h"

class CommandDefaultMessage : public ACommand
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::DEFAULT_MESSAGE; }

	void run(std::string msg, GuestData guest, uint32_t lastGuestUserId, bool isAdmin = false)
	{
		std::ostringstream reply;
		if (guest.userId != lastGuestUserId)
		{
			reply << (isAdmin ? "$  " : ">  ") << guest.name << " \t (#" << guest.userId << "):\n";
		}

		reply << "\t\t " << msg << "\0";

		_replyMessage = reply.str();
		reply.clear();
		reply.flush();
	}
};

