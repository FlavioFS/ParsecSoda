#include "ChatLog.h"

void ChatLog::logCommand(string message)
{
	commandMutex.lock();

	tryCleanOldCommands();
	_commandLog.push_back(message);

	commandMutex.unlock();
}

void ChatLog::logMessage(string message)
{
	if (
		message.size() > 0 &&
		message[0] != '!' &&
		message[0] != '@' &&
		message[0] != '['
	)
	{
		messageMutex.lock();

		tryCleanOldMessages();
		_messageLog.push_back(message);

		messageMutex.unlock();
	}
	else
	{
		logCommand(message);
	}
}

vector<string>& ChatLog::getCommandLog()
{
	return _commandLog;
}

vector<string>& ChatLog::getMessageLog()
{
	return _messageLog;
}

void ChatLog::tryCleanOldCommands()
{
	if (_commandLog.size() > CHATLOG_COMMAND_LENGTH)
	{
		vector<string>::iterator it = _commandLog.begin();
		_commandLog.erase(it, it + _commandLog.size() / 2);
	}
}

void ChatLog::tryCleanOldMessages()
{
	if (_messageLog.size() > CHATLOG_MESSAGE_LENGTH)
	{
		vector<string>::iterator it = _messageLog.begin();
		_messageLog.erase(it, it + _messageLog.size() / 2);
	}
}
