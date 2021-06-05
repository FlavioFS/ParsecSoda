#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include "parsec.h"
#include "Stringer.h"
#include "GamepadClient.h"

#include "Commands/ACommand.h"	
#include "Commands/CommandBan.h"
#include "Commands/CommandBonk.h"
#include "Commands/CommandFF.h"
#include "Commands/CommandGameId.h"
#include "Commands/CommandGuests.h"
#include "Commands/CommandDefaultMessage.h"
#include "Commands/CommandIpFilter.h"
#include "Commands/CommandKick.h"
#include "Commands/CommandListCommands.h"
#include "Commands/CommandMic.h"
#include "Commands/CommandMirror.h"
#include "Commands/CommandName.h"
#include "Commands/CommandLimit.h"
#include "Commands/CommandPads.h"
#include "Commands/CommandPrivate.h"
#include "Commands/CommandPublic.h"
#include "Commands/CommandQuit.h"
#include "Commands/CommandSetConfig.h"
#include "Commands/CommandSpeakers.h"
#include "Commands/CommandUnban.h"
#include "Commands/CommandVideoFix.h"

#define BOT_GUESTID 0

class ChatBot
{
public:
	ACommand * identifyUserDataMessage(const char * msg);

	const uint32_t getLastUserId() const;
	void setLastUserId(const uint32_t lastId = BOT_GUESTID);

	const std::string formatGuestConnection(ParsecGuest guest, bool isAdmin = false);

	const std::string formatBannedGuestMessage(ParsecGuest guest);

private:
	static bool msgStartsWith(const char* msg, const char* pattern);
	static bool msgIsEqual(const char* msg, const char* pattern);

	uint32_t _lastUserId = 0;
};

