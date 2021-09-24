#pragma once

#include "ACommandSearchUser.h"
#include <Windows.h>
#include <mmsystem.h>
#include <iostream>
#include <sstream>
#include "parsec.h"
#include "../Dice.h"
#include "../Stopwatch.h"

class CommandBonk : public ACommandSearchUser
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::BONK; }

	CommandBonk(const char* msg, Guest& sender, GuestList& guests, Guest& host)
		: ACommandSearchUser(msg, internalPrefixes(), guests), _sender(sender), _host(host)
	{}

	bool run() override
	{
		if (!_stopwatch.isFinished())
		{
			_replyMessage = std::string() +
				"[ChatBot] | Bonk command is on cooldown: " +
				to_string(_stopwatch.getRemainingTime()/1000) + " seconds left.\0";
			return false;
		}

		ACommandSearchUser::run();

		if (_searchResult != SEARCH_USER_RESULT::FOUND)
		{
			try
			{
				if (_host.userID == stoul(_targetUsername))
				{
					_targetGuest = _host;
					_searchResult = SEARCH_USER_RESULT::FOUND;
				}
			}
			catch (const std::exception&) {}

			if (_searchResult != SEARCH_USER_RESULT::FOUND && _targetUsername.compare(_host.name) == 0)
			{
				_targetGuest = _host;
				_searchResult = SEARCH_USER_RESULT::FOUND;
			}
		}


		bool rv = false;
		static uint8_t BONK_CHANCE = 50;

		switch (_searchResult)
		{
		case SEARCH_USER_RESULT::NOT_FOUND:
			if (Dice::roll(BONK_CHANCE))
				_replyMessage = std::string() + "[ChatBot] | " + _sender.name + " dreams of bonking but the target out of reach.\0";
			else
				_replyMessage = std::string() + "[ChatBot] | " + _sender.name + " yearns for bonking but the victim is not here.\0";
			break;

		case SEARCH_USER_RESULT::FOUND:
			_stopwatch.reset();
			rv = true;
			if (_sender.userID == _targetGuest.userID)
			{
				_replyMessage = std::string() + "[ChatBot] | " + _sender.name + " self-bonked. *Bonk!*\0";
				try
				{
					PlaySound(TEXT("./sfx/bonk-hit.wav"), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
				}
				catch (const std::exception&) {}
			}
			else if (Dice::roll(BONK_CHANCE))
			{
				_replyMessage = std::string() + "[ChatBot] | " + _sender.name + " bonked " + _targetGuest.name + ". *Bonk!*\0";
				try
				{
					PlaySound(TEXT("./sfx/bonk-hit.wav"), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
				}
				catch (const std::exception&) {}
			}
			else
			{
				_replyMessage = std::string() + "[ChatBot] | " + _targetGuest.name + " dodged " + _sender.name + "'s bonk. *Swoosh!*\0";
				try
				{
					PlaySound(TEXT("./sfx/bonk-dodge.wav"), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
				}
				catch (const std::exception&) {}
			}
			break;
		
		case SEARCH_USER_RESULT::FAILED:
		default:
			_replyMessage = "[ChatBot] | Usage: !bonk <username>\nExample: !bonk melon\0";
			break;
		}

		return rv;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!bonk" };
	}

	static void init();

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!bonk " };
	}

	static Stopwatch _stopwatch;

	Guest& _sender;
	Guest& _host;
};

