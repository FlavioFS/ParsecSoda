#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <functional>
#include <mutex>
#include <Windows.h>
#include "MetadataCache.h"

#define CHATLOG_COMMAND_LENGTH 200
#define CHATLOG_MESSAGE_LENGTH 100

using namespace std;

class ChatLog
{
public:
	void logCommand(string message);
	void logMessage(string message);

	void clearCommands();
	void clearMessages();

	void getCommandLog(function<void(vector<string>&)> callback);
	void getMessageLog(function<void(vector<string>&)> callback);

private:
	void tryCleanOldCommands();
	void tryCleanOldMessages();

	vector<string> m_commandLog;
	vector<string> m_messageLog;

	mutex m_commandMutex;
	mutex m_messageMutex;
};

