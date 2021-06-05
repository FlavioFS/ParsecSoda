#include "ChatBot.h"

//ChatBot::ChatBot()
//{
//	_dice.init();
//}

ACommand * ChatBot::identifyUserDataMessage(const char* msg)
{
	const uint32_t previous = this->_lastUserId;

	setLastUserId(BOT_GUESTID);
	if		(msgStartsWith(msg, "/ban"))		return new CommandBan();
	else if	(msgStartsWith(msg, "/bonk"))		return new CommandBonk();
	else if (msgStartsWith(msg, "/ff"))			return new CommandFF();
	else if	(msgStartsWith(msg, "/gameid"))		return new CommandGameId();
	else if (msgStartsWith(msg, "/guests"))		return new CommandGuests();
	else if (CommandIpFilter::containsIp(msg))	return new CommandIpFilter();
	else if (msgStartsWith(msg, "/kick"))		return new CommandKick();
	else if	(msgIsEqual(msg, "/commands"))		return new CommandListCommands();
	else if (msgStartsWith(msg, "/mic"))		return new CommandMic();
	else if (msgIsEqual(msg, "/mirror"))		return new CommandMirror();
	else if (msgStartsWith(msg, "/name"))		return new CommandName();
	else if (msgStartsWith(msg, "/owners"))		return new CommandOwners();
	else if (msgStartsWith(msg, "/pads"))		return new CommandPads();
	else if (msgIsEqual(msg, "/private"))		return new CommandPrivate();
	else if (msgIsEqual(msg, "/public"))		return new CommandPublic();
	else if	(msgIsEqual(msg, "/q"))				return new CommandQuit();
	else if (msgIsEqual(msg, "/setconfig"))		return new CommandSetConfig();
	else if (msgStartsWith(msg, "/speakers"))	return new CommandSpeakers();
	else if (msgStartsWith(msg, "/unban"))		return new CommandUnban();
	else if	(msgIsEqual(msg, "/videofix"))		return new CommandVideoFix();

	this->setLastUserId(previous);
	return new CommandDefaultMessage();
}

const uint32_t ChatBot::getLastUserId() const
{
	return this->_lastUserId;
}

const std::string ChatBot::formatGuestMessage(ParsecGuest guest, const char* msg, bool isAdmin)
{
	std::ostringstream reply;
	if (_lastUserId != guest.userID)
	{
		reply << (isAdmin ? "$  " : ">  ") << guest.name << " \t (#" << guest.userID << "):\n";
	}
	
	reply << "\t\t " << msg << "\0";

	const std::string formattedMessage = reply.str();
	reply.clear();
	reply.flush();

	setLastUserId(guest.userID);

	return formattedMessage;
}

const std::string ChatBot::formatGuestConnection(ParsecGuest guest, bool isAdmin)
{
	setLastUserId(BOT_GUESTID);

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

void ChatBot::setLastUserId(uint32_t lastId)
{
	this->_lastUserId = lastId;
}

//bool ChatBot::findUser(uint32_t targetUserID, ParsecGuest * guests, int guestCount, ParsecGuest *targetUser)
//{
//	ParsecGuest it;
//	for (int i = 0; i < guestCount; i++)
//	{
//		it = guests[i];
//		if (it.userID == targetUserID)
//		{
//			*targetUser = guests[i];
//			return true;
//		}
//	}
//	return false;
//}
//
//bool ChatBot::findUser(std::string targetUserName, ParsecGuest * guests, int guestCount, ParsecGuest *targetUser)
//{
//	const uint64_t minimum_bonk_match = 3;
//	uint64_t closestDistance = STRINGER_MAX_DISTANCE;
//	uint64_t di = STRINGER_MAX_DISTANCE;
//	bool result = false;
//
//	ParsecGuest it;
//	for (int i = 0; i < guestCount; i++)
//	{
//		it = guests[i];
//		di = Stringer::fuzzyDistance(it.name, targetUserName);
//		if (di <= closestDistance && di <= STRINGER_DISTANCE_CHARS(minimum_bonk_match))
//		{
//			closestDistance = di;
//			*targetUser = guests[i];
//			result = true;
//		}
//	}
//
//	return result;
//}