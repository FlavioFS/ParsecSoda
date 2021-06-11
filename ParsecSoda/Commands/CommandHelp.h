#pragma once

#include "ACommand.h"
#include <iostream>

class CommandHelp : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::HELP; }

	CommandHelp(bool &isAdmin)
		: _isAdmin(isAdmin)
	{}

	bool run() override
	{
		const std::string pleb_commands = std::string()
			+ "\n  " + "---- Normal Commands ----"
			+ "\n  " + "!afk\t\t\t\t\t|\tRemoves gamepads from players who left the room."
			+ "\n  " + "!bonk\t\t\t\t |\tBonk another user."
			+ "\n  " + "!commands\t|\tShow command list."
			+ "\n  " + "!ff\t\t\t\t\t\t |\tDrop your gamepads."
			+ "\n  " + "!mirror\t\t\t   |\tToggle mirroring of L-Stick into DPad."
			+ "\n  " + "!pads\t\t\t\t  |\tShow who's holding each gamepad."
			+ "\n  " + "!swap\t\t\t\t |\tReplace your gamepad with another one."
			;

		const std::string admin_commands = std::string()
			+ pleb_commands
			+ "\n  " + ""
			+ "\n  " + "---- Admin Commands ----"
			+ "\n  " + "!ban\t\t\t\t|\tBan a guest."
			+ "\n  " + "!dc\t\t\t\t   |\tDisconnect a specific gamepad."
			+ "\n  " + "!gameid\t\t|\tSet game id."
			+ "\n  " + "!guests\t\t  |\tSet the amount of room slots."
			+ "\n  " + "!kick\t\t\t   |\tKick user from the room."
			+ "\n  " + "!mic\t\t\t\t|\tSet microphone volume."
			+ "\n  " + "!name\t\t\t|\tSet room name."
			+ "\n  " + "!private\t\t |\tMake the room private."
			+ "\n  " + "!public\t\t   |\tMake the room public."
			+ "\n  " + "!q\t\t\t\t\t |\tClose stream."
			+ "\n  " + "!limit\t\t\t   |\tSet the maximum amount of pads a guest can hold."
			+ "\n  " + "!setconfig\t |\tApply config changes."
			+ "\n  " + "!speakers\t  |\tSet speakers volume."
			+ "\n  " + "!unban\t\t   |\tUnban a guest."
			//+ "\n  " + "!videofix\t    |\tRefresh DirectX context."	// Broken (massive memory leaks)
			;

		_replyMessage =
			std::string("[ChatBot] | Command list: ")
			+ (_isAdmin ? admin_commands : pleb_commands)
			+ "\n\0";

		return true;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!help", "!commands" };
	}

protected:
	bool &_isAdmin;
};

