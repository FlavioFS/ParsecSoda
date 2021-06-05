#pragma once

#include <regex>
#include "ACommand.h"
#include "parsec.h"

class CommandIpFilter : public ACommand
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::IP; }

	static bool containsIp(const char* msg)
	{
		std::string message = msg;
		std::smatch match;
		std::regex ipRegex("((\\d{3}|\\d{2})[\\s\\D]+){2}((\\d{3}|\\d{2}|\\d{1})[\\s\\D]+){1}");

		return std::regex_search(message, match, ipRegex);
	}

	void run(Parsec* ps, ParsecGuest sender, BanList* banList)
	{
		banList->ban(GuestData(sender.name, sender.userID));
		ParsecHostKickGuest(ps, sender.id);
		_replyMessage = std::string() + "! [ChatBot] | " + sender.name + " was banned by ChatBot.\n\t\tBEGONE! *MEGA BONK*\0";
	}
};

