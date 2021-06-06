#pragma once

#include <string>

enum class COMMAND_TYPE
{
	INVALID = -1,
	BOT_MESSAGE,
	DEFAULT_MESSAGE,
	BAN,
	BONK,
	COMMANDS,
	DC,
	FF,
	GAMEID,
	GUESTS,
	IP,
	KICK,
	LIMIT,
	MIC,
	MIRROR,
	NAME,
	PADS,
	PRIVATE,
	PUBLIC,
	QUIT,
	SETCONFIG,
	SPEAKERS,
	SWAP,
	UNBAN,
	VIDEOFIX,
	
	// Search + number
	GIVE,		// new

	// Search
	TAKE		// new
};

class ACommand
{
public:
	virtual const COMMAND_TYPE type() const { return COMMAND_TYPE::INVALID; }
	const std::string replyMessage() const { return _replyMessage; }

protected:
	std::string _replyMessage = "";
};