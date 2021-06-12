#pragma once

#include "IWidget.h"
#include "ToggleTexButtonWidget.h"
#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../AppIcons.h"
#include "../AppStyle.h"

class ChatWidget
{
public:
	ChatWidget(AppIcons& icons, Hosting& hosting);
	bool render(AppStyle& style);

	const ImVec2 DEFAULT_BUTTON_SIZE = ImVec2(40, 40);

	#define LOG_BUFFER_LEN 16383
	#define SEND_BUFFER_LEN 1023

private:
	bool isDirty();
	void sendMessage();
	bool setSendBuffer(const char* value);

	// Dependency injection
	AppIcons& _icons;
	Hosting& _hosting;

	// Imported widgets
	ToggleTexButtonWidget _sendBtn;
	
	// Attributes
	string _logBuffer;
	char _sendBuffer[SEND_BUFFER_LEN];
	vector<string>& _chatLog;
};

