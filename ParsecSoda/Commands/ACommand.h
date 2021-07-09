#pragma once

#include <string>
#include <vector>

enum class COMMAND_TYPE
{
	INVALID = -1,
	BOT_MESSAGE,
	DEFAULT_MESSAGE,
	AFK,
	BAN,
	BONK,
	DC,
	FF,
	GAMEID,
	GUESTS,
	HELP,
	IP,
	JOIN,
	KICK,
	LIMIT,
	MIC,
	MIRROR,
	NAME,
	ONE,
	PADS,
	PRIVATE,
	PUBLIC,
	QUIT,
	SETCONFIG,
	SFX,
	SPEAKERS,
	SWAP,
	TAKE,
	UNBAN,
	VIDEOFIX,
	
	// Search + number
	GIVE,		// new
};

class ACommand
{
public:
	virtual const COMMAND_TYPE type() { return COMMAND_TYPE::INVALID; }
	virtual bool run() = 0;
	const std::string replyMessage() const { return _replyMessage; }

protected:
	std::string _replyMessage = "";
};