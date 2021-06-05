#pragma once

#include <string>

enum class COMMAND_TYPE
{
	INVALID = -1,
	DEFAULT_MESSAGE,
	BAN,
	BONK,
	COMMANDS,
	GAMEID,
	GUESTS,
	IP,
	KICK,
	MIC,
	MIRROR,
	NAME,
	OWNERS,
	PADS,
	PRIVATE,
	PUBLIC,
	QUIT,
	SETCONFIG,
	SPEAKERS,
	UNBAN,
	VIDEOFIX,
	
	// Search + number
	GIVE,		// new

	// Search
	TAKE,		// new

	// Self
	FF			// new
};

class ACommand
{
public:
	virtual const COMMAND_TYPE type() const { return COMMAND_TYPE::INVALID; }
	const std::string replyMessage() const { return _replyMessage; }

protected:
	std::string _replyMessage = "";
};