#pragma once

#include <iostream>
#include <sstream>
#include "ACommand.h"
#include "../Guest.h"

class CommandDefaultMessage : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::DEFAULT_MESSAGE; }

	CommandDefaultMessage(const char* msg, Guest &sender, uint32_t lastUserID, bool isAdmin = false, bool isHost = false)
		: _msg(msg), _sender(sender), _lastUserID(lastUserID), _isAdmin(isAdmin), _isHost(isHost)
	{}

	bool run() override
	{
		std::ostringstream reply;
		if (_sender.userID != _lastUserID)
		{
			static string role = "";
			if (_isHost) role = "#  ";
			else if (_isAdmin) role = "$  ";
			else role = ">  ";
			
			if (_sender.isValid())
			{
				reply << role << _sender.name << " \t (#" << _sender.userID << ")";
			}
			else if(_isHost)
			{
				reply << role << "Host";
			}
			else
			{
				reply << role << "Unkown Guest";
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
	bool _isHost;
};