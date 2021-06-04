#pragma once

#include "ACommandSearchUser.h"
#include <iostream>
#include "parsec.h"

class CommandKick : public ACommandSearchUser
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::KICK; }

	void run(const char* msg, Parsec *ps, ParsecGuest sender, ParsecGuest* guests, int guestCount)
	{
		ParsecGuest targetUser;
		SEARCH_USER_RESULT search = ACommandSearchUser::run(msg, "/kick ", guests, guestCount, &targetUser);
		
		switch (search)
		{
		case SEARCH_USER_RESULT::NOT_FOUND:
			_replyMessage = std::string() + "[ChatBot] | " + sender.name + ", I cannot find the user you want to kick.\0";
			break;

		case SEARCH_USER_RESULT::FOUND:
			if (sender.userID == targetUser.userID)
				_replyMessage = std::string() + "[ChatBot] | Thou shall not kick thyself, " + sender.name + " ...\0";
			else
			{
				_replyMessage = std::string() + "[ChatBot] | " + targetUser.name + " was kicked by " + sender.name + "!\0";
				ParsecHostKickGuest(ps, targetUser.id);
			}
			break;
		
		case SEARCH_USER_RESULT::FAILED:
		default:
			_replyMessage = "[ChatBot] | Usage: /kick <username>\nExample: /kick melon\0";
			break;
		}
	}
};

