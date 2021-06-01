#pragma once

#include "ChatCommand.h"
#include <iostream>
#include <sstream>

class ChatGuestMessage : public ChatCommand
{
public:
	const CHAT_COMMAND_TYPE type = CHAT_CMD_USERMESSAGE;
};

