#include "ChatLog.h"

void ChatLog::logCommand(string message)
{
	m_commandMutex.lock();

	tryCleanOldCommands();
	m_commandLog.push_back(message);

	m_commandMutex.unlock();
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
		m_messageMutex.lock();

		tryCleanOldMessages();
		m_messageLog.push_back(message);

		if (MetadataCache::preferences.enableChatSoundNotification)
		{
			try
			{
				PlaySound(TEXT("./sfx/chat.wav"), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
			}
			catch (const std::exception&) {}
		}

		m_messageMutex.unlock();
	}
	else
	{
		logCommand(message);
	}
}

void ChatLog::clearCommands()
{
	m_commandMutex.lock();
	m_commandLog.clear();
	m_commandMutex.unlock();
}

void ChatLog::clearMessages()
{
	m_messageMutex.lock();
	m_messageLog.clear();
	m_messageMutex.unlock();
}

void ChatLog::getCommandLog(function<void(vector<string>&)> callback)
{
	m_commandMutex.lock();

	if (callback)
	{
		callback(m_commandLog);
	}

	m_commandMutex.unlock();
}

void ChatLog::getMessageLog(function<void(vector<string>&)> callback)
{
	m_messageMutex.lock();

	if (callback)
	{
		callback(m_messageLog);
	}

	m_messageMutex.unlock();
}

void ChatLog::tryCleanOldCommands()
{
	if (m_commandLog.size() > CHATLOG_COMMAND_LENGTH)
	{
		vector<string>::iterator it = m_commandLog.begin();
		m_commandLog.erase(it, it + m_commandLog.size() / 2);
	}
}

void ChatLog::tryCleanOldMessages()
{
	if (m_messageLog.size() > CHATLOG_MESSAGE_LENGTH)
	{
		vector<string>::iterator it = m_messageLog.begin();
		m_messageLog.erase(it, it + m_messageLog.size() / 2);
	}
}
