#pragma once

#include <string>

enum class COMMAND_TYPE
{
	INVALID = -1,
	BOT_MESSAGE,
	DEFAULT_MESSAGE,
	AFK,
	BAN,
	BONK,
	COMMANDS,
	DC,
	FF,
	GAMEID,
	GUESTS,
	IP,
	JOIN,
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
	virtual bool run() = 0;

protected:
	std::string _replyMessage = "";
};