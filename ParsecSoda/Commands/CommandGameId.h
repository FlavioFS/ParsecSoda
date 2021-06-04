#pragma once

#include "parsec.h"
#include "ACommandStringArg.h"

class CommandGameId : public ACommandStringArg
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::GAMEID; }

	void run(const char * msg, ParsecHostConfig* config)
	{
		std::string gameId;
		if ( !ACommandStringArg::run(msg, "/gameid ", &gameId) )
		{
			_replyMessage = "[ChatBot] | Usage: /gameid <id>\nExample: /gameid 1RR6JAsP4sdrjUOMEV4i7lVwMht\0";
			return;
		}

		strcpy_s(config->gameID, gameId.c_str());

		_replyMessage = std::string() + "[ChatBot] | Game ID changed:\n" + gameId + "\0";
	}
};
