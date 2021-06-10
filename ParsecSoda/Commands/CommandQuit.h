#pragma once

#include "ACommand.h"

class CommandQuit : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::QUIT; }

	CommandQuit(bool & hostingLoopController)
		: _hostingLoopController(hostingLoopController)
	{}

	bool run() override
	{
		_replyMessage = "[ChatBot] | Closing stream...\0";
		_hostingLoopController = false;
		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!q" };
	}

protected:
	bool& _hostingLoopController;
};

