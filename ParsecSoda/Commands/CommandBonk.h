#pragma once

#include "ACommandSearchUser.h"
#include <iostream>
#include <sstream>
#include "parsec.h"
#include "../Dice.h"

class CommandBonk : public ACommandSearchUser
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::BONK; }

	void run(const char* msg, ParsecGuest sender, ParsecGuest* guests, int guestCount, Dice *dice)
	{
		const uint8_t BONK_CHANCE = 50;

		ParsecGuest targetUser;
		SEARCH_USER_RESULT search = ACommandSearchUser::run(msg, "/bonk ", guests, guestCount, &targetUser);
		
		switch (search)
		{
		case SEARCH_USER_RESULT::NOT_FOUND:
			if (dice->roll(BONK_CHANCE))
				_replyMessage = std::string() + "[ChatBot] | " + sender.name + " dreams of bonking but the target out of reach.\0";
			else
				_replyMessage = std::string() + "[ChatBot] | " + sender.name + " yearns for bonking but the victim is not here.\0";
			break;

		case SEARCH_USER_RESULT::FOUND:
			if (sender.userID == targetUser.userID)
				_replyMessage = std::string() + "[ChatBot] | " + sender.name + " self-bonked. *Bonk!*\0";
			else if (dice->roll(BONK_CHANCE))
				_replyMessage = std::string() + "[ChatBot] | " + sender.name + " bonked " + targetUser.name + ". *Bonk!*\0";
			else
				_replyMessage = std::string() + "[ChatBot] | " + targetUser.name + " dodged " + sender.name + "'s bonk. *Swoosh!*\0";
			break;
		
		case SEARCH_USER_RESULT::FAILED:
		default:
			_replyMessage = "[ChatBot] | Usage: /bonk <username>\nExample: /bonk melon\0";
			break;
		}
	}
};

