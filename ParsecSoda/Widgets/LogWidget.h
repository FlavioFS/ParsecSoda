#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"

class LogWidget
{
public:
	LogWidget(Hosting& hosting);
	bool render();

private:
	string _logBuffer;
	ChatLog& _chatLog;
	size_t _messageCount;
};

