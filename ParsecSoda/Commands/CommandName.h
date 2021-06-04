#pragma once

#include "parsec.h"
#include "ACommandStringArg.h"

class CommandName : public ACommandStringArg
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::NAME; }

	void run(const char * msg, ParsecHostConfig* config)
	{
		std::string roomName;
		if ( !ACommandStringArg::run(msg, "/name ", &roomName) )
		{
			_replyMessage = "[ChatBot] | Usage: /name <roomname>\nExample: /name Let's Play Gauntlet!\0";
			return;
		}

		strcpy_s(config->name, roomName.c_str());

		_replyMessage = std::string() + "[ChatBot] | Room name changed:\n" + roomName + "\0";
	}
};
