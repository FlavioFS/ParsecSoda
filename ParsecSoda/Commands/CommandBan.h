#pragma once

#include "ACommandSearchUser.h"
#include <iostream>
#include "parsec.h"
#include "../BanList.h"

class CommandBan : public ACommandSearchUser
{
public:
	const COMMAND_TYPE type() const { return COMMAND_TYPE::BAN; }

	void run(const char* msg, Parsec *ps, ParsecGuest sender, ParsecGuest* guests, int guestCount, BanList * banList)
	{
		ParsecGuest targetUser;
		SEARCH_USER_RESULT search = ACommandSearchUser::run(msg, "/ban ", guests, guestCount, &targetUser);
		
		switch (search)
		{
		case SEARCH_USER_RESULT::NOT_FOUND:
			_replyMessage = std::string() + "[ChatBot] | " + sender.name + ", I cannot find the user you want to ban.\0";
			break;

		case SEARCH_USER_RESULT::FOUND:
			if (sender.userID == targetUser.userID)
				_replyMessage = std::string() + "[ChatBot] | Thou shall not ban thyself, " + sender.name + " ...\0";
			else
			{
				_replyMessage = std::string() + "[ChatBot] | " + targetUser.name + " was banned by " + sender.name + "!\0";
				banList->ban( GuestData(targetUser.name, targetUser.userID) );
				ParsecHostKickGuest(ps, targetUser.id);
			}
			break;
		
		case SEARCH_USER_RESULT::FAILED:
		default:
			_replyMessage = "[ChatBot] | Usage: /ban <username>\nExample: /ban melon\0";
			break;
		}
	}
};

