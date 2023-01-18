#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <mutex>

#define CHATLOG_COMMAND_LENGTH 200
#define CHATLOG_MESSAGE_LENGTH 100

using namespace std;

class ChatLog
{
public:
	void logCommand(string message);
	void logMessage(string message);

	vector<string>& getCommandLog();
	vector<string>& getMessageLog();

	mutex commandMutex;
	mutex messageMutex;

private:
	void tryCleanOldCommands();
	void tryCleanOldMessages();

	vector<string> _commandLog;
	vector<string> _messageLog;
};

