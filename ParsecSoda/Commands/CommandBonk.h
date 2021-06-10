#pragma once

#include "ACommandSearchUser.h"
#include <iostream>
#include <sstream>
#include "parsec.h"
#include "../Dice.h"

class CommandBonk : public ACommandSearchUser
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::BONK; }

	CommandBonk(const char* msg, Guest& sender, GuestList& guests, Dice &dice)
		: ACommandSearchUser(msg, internalPrefixes(), guests), _sender(sender), _dice(dice)
	{}

	bool run() override
	{
		ACommandSearchUser::run();

		bool rv = false;
		const uint8_t BONK_CHANCE = 50;

		switch (_searchResult)
		{
		case SEARCH_USER_RESULT::NOT_FOUND:
			if (_dice.roll(BONK_CHANCE))
				_replyMessage = std::string() + "[ChatBot] | " + _sender.name + " dreams of bonking but the target out of reach.\0";
			else
				_replyMessage = std::string() + "[ChatBot] | " + _sender.name + " yearns for bonking but the victim is not here.\0";
			break;

		case SEARCH_USER_RESULT::FOUND:
			rv = true;
			if (_sender.userID == _targetGuest.userID)
				_replyMessage = std::string() + "[ChatBot] | " + _sender.name + " self-bonked. *Bonk!*\0";
			else if (_dice.roll(BONK_CHANCE))
				_replyMessage = std::string() + "[ChatBot] | " + _sender.name + " bonked " + _targetGuest.name + ". *Bonk!*\0";
			else
				_replyMessage = std::string() + "[ChatBot] | " + _targetGuest.name + " dodged " + _sender.name + "'s bonk. *Swoosh!*\0";
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

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!bonk " };
	}

	Guest _sender;
	Dice& _dice;
};

