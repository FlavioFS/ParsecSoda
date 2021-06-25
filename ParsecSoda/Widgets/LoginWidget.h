#pragma once

#include <thread>
#include <Windows.h>
#include <shellapi.h>
#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppStyle.h"
#include "../globals/AppIcons.h"
#include "TitleTooltipWidget.h"
#include "LoadingRingWidget.h"
#include "AlertWidget.h"
#include "ConfirmPopupWidget.h"

class LoginWidget
{
public:
	LoginWidget(Hosting& hosting);
	void render(bool& isValidSession);

private:
	void renderLoginTooltip();
	Hosting& _hosting;
	thread _loginThread;
	bool _isLoginLocked;
	void attemptLogin();
	char _email[128] = "";
	char _password[128] = "";
	string _sessionError;
	int _sessionStatus;
	bool _showError = false;
};

