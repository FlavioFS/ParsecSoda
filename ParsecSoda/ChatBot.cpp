#include "ChatBot.h"


ACommand * ChatBot::identifyUserDataMessage(const char* msg, Guest &sender, bool &isAdmin)
{
	const uint32_t previous = this->_lastUserId;

	setLastUserId(BOT_GUESTID);

	// Pleb commands
	if		(msgIsEqual(msg, CommandAFK::prefixes()))			return new CommandAFK(_guests, _gamepadClient);
	else if	(msgStartsWith(msg, CommandBonk::prefixes()))		return new CommandBonk(msg, sender, _guests, _dice);
	else if (msgIsEqual(msg, CommandFF::prefixes()))			return new CommandFF(sender, _gamepadClient);
	else if	(msgIsEqual(msg, CommandHelp::prefixes()))			return new CommandHelp(isAdmin);
	else if (CommandIpFilter::containsIp(msg))					return new CommandIpFilter(msg, sender, _parsec, _ban);
	else if (msgIsEqual(msg, CommandJoin::prefixes()))			return new CommandJoin();
	else if (msgIsEqual(msg, CommandMirror::prefixes()))		return new CommandMirror(sender, _gamepadClient);
	else if (msgIsEqual(msg, CommandPads::prefixes()))			return new CommandPads(_gamepadClient);
	else if (msgStartsWith(msg, CommandSwap::prefixes()))		return new CommandSwap(msg, sender, _gamepadClient);

	// Admin commands
	if (isAdmin)
	{
		if		(msgStartsWith(msg, CommandBan::prefixes()))		return new CommandBan(msg, sender, _parsec, _guests, _ban);
		else if (msgStartsWith(msg, CommandDC::prefixes()))			return new CommandDC(msg, _gamepadClient);
		else if	(msgStartsWith(msg, CommandGameId::prefixes()))		return new CommandGameId(msg, _hostConfig);
		else if (msgStartsWith(msg, CommandGuests::prefixes()))		return new CommandGuests(msg, _hostConfig);
		else if (msgStartsWith(msg, CommandKick::prefixes()))		return new CommandKick(msg, sender, _parsec, _guests);
		else if (msgStartsWith(msg, CommandLimit::prefixes()))		return new CommandLimit(msg, _guests, _gamepadClient);
		else if (msgStartsWith(msg, CommandMic::prefixes()))		return new CommandMic(msg, _audioMixer);
		else if (msgStartsWith(msg, CommandName::prefixes()))		return new CommandName(msg, _hostConfig);
		else if (msgIsEqual(msg, CommandPrivate::prefixes()))		return new CommandPrivate(_hostConfig);
		else if (msgIsEqual(msg, CommandPublic::prefixes()))		return new CommandPublic(_hostConfig);
		else if	(msgIsEqual(msg, CommandQuit::prefixes()))			return new CommandQuit(_hostingLoopController);
		else if (msgIsEqual(msg, CommandSetConfig::prefixes()))		return new CommandSetConfig(_parsec, &_hostConfig, _parsecSession.sessionId.c_str());
		else if (msgStartsWith(msg, CommandStrip::prefixes()))		return new CommandStrip(msg, sender, _gamepadClient);
		else if (msgStartsWith(msg, CommandSpeakers::prefixes()))	return new CommandSpeakers(msg, _audioMixer);
		else if (msgStartsWith(msg, CommandUnban::prefixes()))		return new CommandUnban(msg, sender, _ban);
		//else if	(msgIsEqual(msg, CommandVideoFix::prefixes()))		return new CommandVideoFix(_dx11); // Broken (massive memory leaks) & obsolete
	}

	this->setLastUserId(previous);
	return new CommandDefaultMessage(msg, sender, _lastUserId, isAdmin);
}

const uint32_t ChatBot::getLastUserId() const
{
	return this->_lastUserId;
}

const std::string ChatBot::formatGuestConnection(Guest guest, ParsecGuestState state)
{
	setLastUserId(BOT_GUESTID);

	std::ostringstream reply;
	if (state == GUEST_CONNECTED)
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

const std::string ChatBot::formatBannedGuestMessage(Guest guest)
{
	std::ostringstream reply;
	reply << "[ChatBot] | None shall pass! Banned guests don't join us:\n\t\t" << guest.name << " \t (#" << guest.userID << ")\0";

	return reply.str();
}

CommandBotMessage ChatBot::sendBotMessage(const char* msg)
{
	CommandBotMessage message(msg);
	message.run();
	setLastUserId(BOT_GUESTID);
	return message;
}

bool ChatBot::msgStartsWith(const char* msg, const char * pattern)
{
	return Stringer::startsWithPattern(msg, pattern);
}

bool ChatBot::msgStartsWith(const char* msg, vector<const char*> patterns)
{
	vector<const char*>::iterator pi = patterns.begin();
	for (; pi != patterns.end(); ++pi)
	{
		if (Stringer::startsWithPattern(msg, *pi))
		{
			return true;
		}
	}
	return false;
}

bool ChatBot::msgIsEqual(const char * msg, const char * pattern)
{
	return (strcmp(msg, pattern) == 0);
}

bool ChatBot::msgIsEqual(const char* msg, vector<const char*> patterns)
{
	vector<const char*>::iterator pi = patterns.begin();
	for (; pi != patterns.end(); ++pi)
	{
		if ((strcmp(msg, *pi) == 0))
		{
			return true;
		}
	}
	return false;
}

void ChatBot::setLastUserId(uint32_t lastId)
{
	this->_lastUserId = lastId;
}