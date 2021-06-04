#pragma once

#include <sstream>
#include "parsec.h"
#include "ACommandIntegerArg.h"

class CommandGuests : public ACommandIntegerArg
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::GUESTS; }

	void run(const char * msg, ParsecHostConfig* config)
	{
		int guestCount = -1;
		if ( !ACommandIntegerArg::run(msg, "/guests ", &guestCount) )
		{
			_replyMessage = "[ChatBot] | Usage: /guests <number>\nExample: /guests 7\0";
			return;
		}

		config->maxGuests = guestCount;
		std::ostringstream reply;
		reply << "[ChatBot] | Max guests set to " << guestCount << "\0";
		_replyMessage = reply.str();
	}
};
