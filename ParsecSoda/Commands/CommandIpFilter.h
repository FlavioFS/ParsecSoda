#pragma once

#include <regex>
#include <Windows.h>
#include <mmsystem.h>
#include "ACommand.h"
#include "parsec-dso.h"
#include "../Guest.h"

class CommandIpFilter : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::IP; }

	CommandIpFilter(const char* msg, Guest& sender, ParsecDSO* parsec, BanList &ban, bool isHost = false)
		: _msg(msg), _sender(sender), _parsec(parsec), _ban(ban), _isHost(isHost)
	{}

	static bool containsIp(const char* msg)
	{
		std::string message = msg;
		std::smatch match;
		std::regex ipRegex("((\\d{3}|\\d{2})[\\s\\D]+){2}((\\d{3}|\\d{2}|\\d{1})[\\s\\D]+){1}");

		return std::regex_search(message, match, ipRegex);
	}

	bool run() override
	{
		if (!_isHost)
		{
			ParsecHostKickGuest(_parsec, _sender.id);
		}
		_ban.ban(GuestData(_sender.name, _sender.userID));
		_replyMessage = std::string() + "! [ChatBot] | " + _sender.name + " was banned by ChatBot.\n\t\tBEGONE! *MEGA BONK*\0";

		try
		{
			PlaySound(TEXT("./sfx/banido.wav"), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
		}
		catch (const std::exception&) {}

		return true;
	}

protected:
	string _msg;
	ParsecDSO* _parsec;
	Guest& _sender;
	BanList& _ban;
	bool _isHost;
};