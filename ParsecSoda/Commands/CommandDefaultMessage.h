#pragma once

#include <iostream>
#include <sstream>
#include "ACommand.h"
#include "../Guest.h"

class CommandDefaultMessage : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::DEFAULT_MESSAGE; }

	CommandDefaultMessage(const char* msg, Guest &sender, uint32_t lastUserID, bool isAdmin = false)
		: _msg(msg), _sender(sender), _lastUserID(lastUserID), _isAdmin(isAdmin)
	{}

	bool run() override
	{
		std::ostringstream reply;
		if (_sender.userID != _lastUserID)
		{
			static string role = "";
			if (_sender.isHost) role = "#  ";
			else if (_isAdmin) role = "$  ";
			else role = ">  ";
			

			reply << role << _sender.name;
			if (!_sender.isHost)
			{
				reply << " \t (#" << _sender.userID << ")";
			}

			reply << ":\n";
		}

		reply << "\t\t " << _msg << "\0";

		_replyMessage = reply.str();
		reply.clear();
		reply.flush();

		return true;
	}

protected:
	string _msg;
	Guest &_sender;
	uint32_t _lastUserID;
	bool _isAdmin;
};