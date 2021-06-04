#pragma once

#include "ACommand.h"
#include <iostream>

class CommandListCommands : public ACommand
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::COMMANDS; }

	void run(bool isAdmin)
	{
		const std::string pleb_commands = std::string()
			+ "\n  " + "---- Normal Commands ----"
			+ "\n  " + "/bonk\t\t\t |\tBonk another user."
			;

		const std::string admin_commands = std::string()
			+ pleb_commands
			+ "\n  " + ""
			+ "\n  " + "---- Admin Commands ----"
			+ "\n  " + "/gameid\t\t|\tSet game id."
			+ "\n  " + "/guests\t\t  |\tSet the amount of room slots."
			+ "\n  " + "/kick\t\t\t   |\tKick user from the room."
			+ "\n  " + "/mic\t\t\t\t|\tSet microphone volume."
			+ "\n  " + "/name\t\t\t|\tSet room name."
			+ "\n  " + "/private\t\t |\tMake the room private."
			+ "\n  " + "/public\t\t   |\tMake the room public."
			+ "\n  " + "/q\t\t\t\t\t |\tClose stream."
			+ "\n  " + "/pads\t\t\t  |\tSet the maximum amount of pads a guest can hold."
			+ "\n  " + "/setconfig\t |\tApply config changes."
			+ "\n  " + "/speakers\t |\tSet microphone volume."
			;

		_replyMessage =
			std::string("[ChatBot] | Command list: ")
			+ (isAdmin ? admin_commands : pleb_commands)
			+ "\n\0";
	}
};

