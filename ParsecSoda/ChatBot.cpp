#include "ChatBot.h"


ACommand * ChatBot::identifyUserDataMessage(const char* msg)
{
	const uint32_t previous = this->_lastUserId;

	setLastUserId(BOT_GUESTID);
	if		(msgStartsWith(msg, "/ban"))		return new CommandBan();
	else if	(msgStartsWith(msg, "/bonk"))		return new CommandBonk();
	else if (msgStartsWith(msg, "/dc"))			return new CommandDC();
	else if (msgIsEqual(msg, "/ff"))			return new CommandFF();
	else if	(msgStartsWith(msg, "/gameid"))		return new CommandGameId();
	else if (msgStartsWith(msg, "/guests"))		return new CommandGuests();
	else if (CommandIpFilter::containsIp(msg))	return new CommandIpFilter();
	else if (msgStartsWith(msg, "/kick"))		return new CommandKick();
	else if	(msgIsEqual(msg, "/commands"))		return new CommandListCommands();
	else if (msgStartsWith(msg, "/mic"))		return new CommandMic();
	else if (msgIsEqual(msg, "/mirror"))		return new CommandMirror();
	else if (msgStartsWith(msg, "/name"))		return new CommandName();
	else if (msgIsEqual(msg, "/pads"))			return new CommandPads();
	else if (msgStartsWith(msg, "/swap"))		return new CommandSwap();
	else if (msgStartsWith(msg, "/limit"))		return new CommandLimit();
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

CommandBotMessage ChatBot::sendBotMessage(const char* msg)
{
	CommandBotMessage message;
	message.run(msg);
	setLastUserId(BOT_GUESTID);
	return message;
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