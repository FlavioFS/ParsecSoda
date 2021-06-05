#pragma once

#include <iostream>
#include <sstream>
#include "ACommand.h"
#include "../GuestData.h"

class CommandDefaultMessage : public ACommand
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::DEFAULT_MESSAGE; }

	void run(std::string msg, ParsecGuest sender, uint32_t lastUserId, bool isAdmin = false)
	{
		std::ostringstream reply;
		if (sender.userID != lastUserId)
		{
			reply << (isAdmin ? "$  " : ">  ") << sender.name << " \t (#" << sender.userID << "):\n";
		}

		reply << "\t\t " << msg << "\0";

		_replyMessage = reply.str();
		reply.clear();
		reply.flush();
	}
};

