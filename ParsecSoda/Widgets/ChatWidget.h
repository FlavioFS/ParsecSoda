#pragma once

#include "ToggleIconButtonWidget.h"
#include "TitleTooltipWidget.h"
#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "../Hosting.h"
#include "../Stopwatch.h"
#include <functional>
#include "SwitchWidget.h"

class ChatWidget
{
public:
	ChatWidget(Hosting& hosting, function<void(void)> onMessageCallback = nullptr);
	bool render();

	const ImVec2 DEFAULT_BUTTON_SIZE = ImVec2(40, 40);

	#define LOG_BUFFER_LEN 16383
	#define SEND_BUFFER_LEN 1023

private:
	bool renderTopBar(bool& isWindowLocked, bool& isClearChat);
	void renderOptionsMenu();

	bool isDirty();
	void sendMessage();
	bool setSendBuffer(const char* value);
	void toClipboard(const string& message);

	// Dependency injection
	Hosting& _hosting;

	// Attributes
	string _logBuffer;
	char _sendBuffer[SEND_BUFFER_LEN];
	char _lastBuffer[SEND_BUFFER_LEN];
	char _previewBuffer[SEND_BUFFER_LEN];
	ChatLog& _chatLog;
	size_t _messageCount;
	function<void(void)> _onMessageCallback;
};