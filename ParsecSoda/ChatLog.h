#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <regex>

#define CHATLOG_COMMAND_LENGTH 50
#define CHATLOG_MESSAGE_LENGTH 100

using namespace std;

class ChatLog
{
public:
	void logCommand(string message);
	void logMessage(string message);

	vector<string>& getCommandLog();
	vector<string>& getMessageLog();

private:
	void tryCleanOldCommands();
	void tryCleanOldMessages();

	vector<string> _commandLog;
	vector<string> _messageLog;
};

