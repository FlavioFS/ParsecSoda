#include "ChatBot.h"

ChatBot::ChatBot()
{
	_dice.init();
}

//ChatCommand ChatBot::identifyMessage(const char* msg)
//{
//	if (msgIsEqual(msg, "/setconfig"))
//
//
//	return ChatCommand();
//}


bool ChatBot::isTrollMessage(const char* msg, ParsecGuest guest, std::string* replyMsg)
{
	std::string message = msg;
	std::smatch match;
	std::regex ipRegex("((\\d{3}|\\d{2})[\\s\\D]+){2}((\\d{3}|\\d{2}|\\d{1})[\\s\\D]+){1}");

	if (std::regex_search(message, match, ipRegex))
	{
		*replyMsg = std::string() + "! [ChatBot] | " + guest.name  + " was kicked by chat bot. BEGONE! *MEGA BONK*\0";
		return true;
	}

	return false;
}

bool ChatBot::isSetConfigMessage(const char * msg, std::string* replyMsg)
{
	if (msgIsEqual(msg, "/setconfig"))
	{
		setLastGuestID(BOT_GUESTID);
		*replyMsg = "[ChatBot] | Room settings applied.\0";
		return true;
	}
	
	return false;
}

bool ChatBot::isPadsLimitMessage(const char* msg, ParsecGuest* guests, int guestCount, GamepadClient* padClient, std::string* replyMsg)
{
	if (msgStartsWith(msg, "/pads"))
	{
		setLastGuestID(BOT_GUESTID);

		if (!msgStartsWith(msg, "/pads "))
		{
			*replyMsg = "[ChatBot] | Usage: /pads <username> <number>\nExample: /pads melon 2\0";
			return false;
		}

		std::string str = msg;
		try
		{
			size_t cmdSize = strlen("/pads ");
			
			std::string argNumber = std::string() + str.back();
			int padCount = std::stoi(argNumber);

			std::string args = str.substr(strlen("/pads "));
			std::string username = args.substr(0, args.size()-2);

			ParsecGuest target;
			bool found = findUser(username, guests, guestCount, &target);
			if (!found)
			{
				*replyMsg = "[ChatBot] | Usage: /pads <number> <username>\nExample: /pads melon 2\0";
				return false;
			}

			padClient->setLimit(target.userID, padCount);

			std::ostringstream reply;
			reply << "[ChatBot] | " << target.name << " gamepad limit set to " << padCount << "\0";
			*replyMsg = reply.str().c_str();

			return true;
		}
		catch (const std::exception&)
		{
			*replyMsg = "[ChatBot] | Usage: /pads <number> <username>\nExample: /pads melon 2\0";
			return false;
		}
	}
}

bool ChatBot::isVolumeMessage(const char * msg, std::string * replyMsg, float *volume, const char * command, const char * alias)
{
	if (msgStartsWith(msg, command))
	{
		setLastGuestID(BOT_GUESTID);
		std::string errorDefaultMsg = *replyMsg = std::string() + "[ChatBot] | Usage: " + command + " <integer in range [0, 100]>\nExample: " + command + " 42\0";


		std::string cmd = std::string() + command + " ";
		if (!msgStartsWith(msg, cmd.c_str()))
		{
			*replyMsg = errorDefaultMsg;
			return false;
		}

		std::string str = msg;
		try
		{
			size_t cmdSize = cmd.length();
			int volumeI = std::stoi(str.substr(cmdSize));
			if (volumeI < 0 || volumeI > 100)
			{
				*replyMsg = errorDefaultMsg;
				return false;
			}

			*volume = volumeI / 100.0;

			std::ostringstream reply;
			reply << "[ChatBot] | " << alias <<" volume set to " << volumeI << "%\0";
			*replyMsg = reply.str().c_str();

			return true;
		}
		catch (const std::exception&)
		{
			*replyMsg = errorDefaultMsg;
			return false;
		}
	}

	return false;
}

bool ChatBot::isMicVolumeMessage(const char * msg, std::string * replyMsg, float *volume)
{
	return isVolumeMessage(msg, replyMsg, volume, "/mic", "Microphone");
}

bool ChatBot::isSpeakersVolumeMessage(const char * msg, std::string * replyMsg, float *volume)
{
	return isVolumeMessage(msg, replyMsg, volume, "/speakers", "Speakers");
}

bool ChatBot::isQuitMessage(const char * msg, std::string* replyMsg)
{
	if (msgIsEqual(msg, "/q"))
	{
		setLastGuestID(BOT_GUESTID);
		*replyMsg = "[ChatBot] | Closing stream...\0";
		return true;
	}

	return false;
}

bool ChatBot::isDX11RefreshMessage(const char * msg, std::string* replyMsg)
{
	if (msgIsEqual(msg, "/dx11refresh"))
	{
		setLastGuestID(BOT_GUESTID);
		*replyMsg = "[ChatBot] | Refreshing Directx11...\0";
		return true;
	}

	return false;
}

void ChatBot::processCommandsListMessage(const char * msg, bool isAdmin, std::string * replyMsg)
{
	if (msgIsEqual(msg, "/commands"))
	{
		setLastGuestID(BOT_GUESTID);

		const std::vector<const char*> pleb_commands = {
			"---- Normal Commands ----",
			"/bonk\t\t\t |\tBonk another user."
		};

		const std::vector<const char*> admin_commands = {
			"---- Normal Commands ----",
			"/bonk\t\t\t |\tBonk another user.",
			"",
			"---- Admin Commands ----",
			"/gameid\t\t|\tSet game id.",
			"/guests\t\t  |\tSet the amount of room slots.",
			"/kick\t\t\t   |\tKick user from the room.",
			"/mic\t\t\t\t|\tSet microphone volume.",
			"/name\t\t\t|\tSet room name.",
			"/private\t\t |\tMake the room private.",
			"/public\t\t   |\tMake the room public.",
			"/q\t\t\t\t\t |\tClose stream.",
			"/pads\t\t\t  |\tSet the maximum amount of pads a guest can hold.",
			"/setconfig\t |\tApply config changes.",
			"/speakers\t |\tSet microphone volume."
		};

		std::ostringstream reply;
		reply << "[ChatBot] | Command list: ";

		std::vector<const char*> cmds = isAdmin ? admin_commands : pleb_commands;

		for (size_t i = 0; i < cmds.size(); i++)
		{
			reply << "\n  " << cmds[i];
		}
		reply << "\n\0";

		*replyMsg = reply.str().c_str();
	}
}

bool ChatBot::tryBonkMessage(const char *msg, ParsecGuest sender, ParsecGuest * guests, int guestCount, std::string* replyMsg)
{
	if (msgStartsWith(msg, "/bonk"))
	{
		setLastGuestID(BOT_GUESTID);

		if (!msgStartsWith(msg, "/bonk "))
		{

			*replyMsg = "[ChatBot] | Usage: /bonk <username>\nExample: /bonk melon\0";

			return false;
		}

		std::string str = msg;
		try
		{
			size_t cmdSize = strlen("/bonk ");
			std::string targetUsername = str.substr(cmdSize);

			ParsecGuest targetUser;
			bool found = findUser(targetUsername, guests, guestCount, &targetUser);
			if (found)
			{
				std::ostringstream reply;

				if (sender.userID == targetUser.userID)
				{
					reply << "[ChatBot] | " << sender.name << " self-bonked. *Bonk!*\0";
				}
				else
				{
					if (_dice.roll(BONK_CHANCE))
					{
						reply << "[ChatBot] | " << sender.name << " bonked " << targetUser. name << ". *Bonk!*\0";
					}
					else
					{
						reply << "[ChatBot] | " << targetUser.name << " dodged " << sender.name << "'s bonk. *Swoosh!*\0";
					}
				}

				*replyMsg = reply.str();

				return true;
			}
			else
			{
				std::ostringstream reply;
				reply << "[ChatBot] | User " << sender.name << " wants to bonk someone not present in this room.\0";
				*replyMsg = reply.str();

				return false;
			}
		}
		catch (const std::exception&)
		{
			*replyMsg = "[ChatBot] | Usage: /bonk <username>\nExample: /bonk melon\0";

			return false;
		}
	}
}

bool ChatBot::tryKickMessage(const char* msg, ParsecGuest sender, ParsecGuest* guests, int guestCount, std::string* replyMsg, ParsecGuest* target)
{
	if (msgStartsWith(msg, "/kick"))
	{
		setLastGuestID(BOT_GUESTID);

		if (!msgStartsWith(msg, "/kick "))
		{

			*replyMsg = "[ChatBot] | Usage: /kick <username>\nExample: /kick melon\0";

			return false;
		}

		std::string str = msg;
		try
		{
			size_t cmdSize = strlen("/kick ");
			std::string targetUsername = str.substr(cmdSize);

			ParsecGuest targetUser;
			bool found = findUser(targetUsername, guests, guestCount, &targetUser);
			if (found)
			{
				std::ostringstream reply;
				*target = targetUser;

				if (sender.userID == targetUser.userID)
				{
					reply << "[ChatBot] | Thou shall not kick thyself, " << sender.name << " ...\0";
				}
				else
				{
					reply << "[ChatBot] | " << targetUser.name << " was kicked by " << sender.name << "!\0";
				}

				*replyMsg = reply.str();

				return true;
			}
			else
			{
				std::ostringstream reply;
				reply << "[ChatBot] | " << sender.name << ", I cannot find the user you want to kick.\0";
				*replyMsg = reply.str();

				return false;
			}
		}
		catch (const std::exception&)
		{
			*replyMsg = "[ChatBot] | Usage: /kick <username>\nExample: /kick melon\0";

			return false;
		}
	}

	return false;
}

bool ChatBot::tryBanMessage(const char* msg, ParsecGuest sender, ParsecGuest* guests, int guestCount, std::string* replyMsg, ParsecGuest* target)
{
	return false;
}

bool ChatBot::processHostConfigMessage(const char * msg, ParsecHostConfig *config, std::string* replyMsg)
{
	if (msgIsEqual(msg, "/public"))
	{
		setLastGuestID(BOT_GUESTID);

		config->publicGame = true;
		*replyMsg = "[ChatBot] | Room set to public.\0";
		
		return true;
	}

	if (msgIsEqual(msg, "/private"))
	{
		setLastGuestID(BOT_GUESTID);

		config->publicGame = false;
		*replyMsg = "[ChatBot] | Room set to private.\0";
		
		return true;
	}

	if (msgStartsWith(msg, "/name"))
	{
		setLastGuestID(BOT_GUESTID);

		if (!msgStartsWith(msg, "/name "))
		{
			*replyMsg = "[ChatBot] | Usage: /name <roomname>\nExample: /name Let's Play Gauntlet!\0";

			return false;
		}

		size_t cmdSize = strlen("/name ");
		std::string str = msg;
		std::string name = str.substr(cmdSize);
		
		// Replaces \ + n with \n
		size_t index = 0;
		while (true)
		{
			index = name.find("\\n", index);
			if (index == std::string::npos) break;

			name.replace(index, 2, "\n");
			index++;
		}
		
		
		std::ostringstream reply;
		strcpy_s(config->name, name.c_str());
		reply << "[ChatBot] | Room name changed:\n" << name << "\0";
		*replyMsg = reply.str().c_str();
		
		return true;
	}

	if (msgStartsWith(msg, "/gameid"))
	{
		setLastGuestID(BOT_GUESTID);

		if (!msgStartsWith(msg, "/gameid "))
		{
			*replyMsg = "[ChatBot] | Usage: /gameid <id>\nExample: /gameid 1RR6JAsP4sdrjUOMEV4i7lVwMht\0";

			return false;
		}

		size_t cmdSize = strlen("/gameid ");
		std::string str = msg;
		std::string gameid = str.substr(cmdSize);
		std::ostringstream reply;
		strcpy_s(config->gameID, gameid.c_str());
		reply << "[ChatBot] | Game ID changed:\n" << gameid << "\0";
		*replyMsg = reply.str().c_str();

		return true;
	}

	if (msgStartsWith(msg, "/guests"))
	{
		setLastGuestID(BOT_GUESTID);

		if (!msgStartsWith(msg, "/guests "))
		{
			*replyMsg = "[ChatBot] | Usage: /guests <number>\nExample: /guests 7\0";

			return false;
		}

		std::string str = msg;
		try
		{
			size_t cmdSize = strlen("/guests ");
			int guestCount = std::stoi(str.substr(cmdSize));
			config->maxGuests = guestCount;

			std::ostringstream reply;
			reply << "[ChatBot] | Max users set to " << guestCount << "\0";
			*replyMsg = reply.str().c_str();
			setLastGuestID(BOT_GUESTID);
			
			return true;
		}
		catch (const std::exception&)
		{
			*replyMsg = "[ChatBot] | Usage: /guests <number>\nExample: /guests 7\0";
			setLastGuestID(BOT_GUESTID);

			return false;
		}
	}

	return false;
}

const std::string ChatBot::formatGuestMessage(ParsecGuest guest, const char* msg, bool isAdmin)
{
	std::ostringstream reply;
	if (lastGuestID != guest.userID)
	{
		reply << (isAdmin ? "$  " : ">  ") << guest.name << " \t (#" << guest.userID << "):\n";
	}
	
	reply << "\t\t " << msg << "\0";

	const std::string formattedMessage = reply.str();
	reply.clear();
	reply.flush();

	setLastGuestID(guest.userID);

	return formattedMessage;
}

const std::string ChatBot::formatGuestConnection(ParsecGuest guest, bool isAdmin)
{
	setLastGuestID(BOT_GUESTID);

	std::ostringstream reply;
	if (guest.state == GUEST_CONNECTED)
	{
		reply << "@ >>  joined \t " << guest.name << " \t(#" << guest.userID << ")\0";
	}
	else
	{
		reply << "! <<  quit \t\t  " << guest.name << " \t(#" << guest.userID << ")\0";
	}

	const std::string formattedMessage = reply.str();
	reply.clear();
	reply.flush();
	return formattedMessage;
}

const std::string ChatBot::formatBannedGuestMessage(ParsecGuest guest)
{
	std::ostringstream reply;
	reply << "[ChatBot] | None shall pass! Banned guests don't join us:\n\t\t" << guest.name << " \t (#" << guest.userID << ")\0";

	return reply.str();
}

bool ChatBot::msgStartsWith(const char* msg, const char * pattern)
{
	return Stringer::startsWithPattern(msg, pattern);
}

bool ChatBot::msgIsEqual(const char * msg, const char * pattern)
{
	return (strcmp(msg, pattern) == 0);
}

void ChatBot::setLastGuestID(uint32_t lastID)
{
	this->lastGuestID = lastID;
}

bool ChatBot::findUser(uint32_t targetUserID, ParsecGuest * guests, int guestCount, ParsecGuest *targetUser)
{
	ParsecGuest it;
	for (int i = 0; i < guestCount; i++)
	{
		it = guests[i];
		if (it.userID == targetUserID)
		{
			*targetUser = guests[i];
			return true;
		}
	}
	return false;
}

bool ChatBot::findUser(std::string targetUserName, ParsecGuest * guests, int guestCount, ParsecGuest *targetUser)
{
	const uint64_t minimum_bonk_match = 3;
	uint64_t closestDistance = STRINGER_MAX_DISTANCE;
	uint64_t di = STRINGER_MAX_DISTANCE;
	bool result = false;

	ParsecGuest it;
	for (int i = 0; i < guestCount; i++)
	{
		it = guests[i];
		di = Stringer::fuzzyDistance(it.name, targetUserName);
		if (di <= closestDistance && di <= STRINGER_DISTANCE_CHARS(minimum_bonk_match))
		{
			closestDistance = di;
			*targetUser = guests[i];
			result = true;
		}
	}

	return result;
}