#pragma once

#include <thread>
#include <Windows.h>
#include <shellapi.h>
#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppStyle.h"
#include "../globals/AppIcons.h"
#include "../globals/AppColors.h"
#include "TitleTooltipWidget.h"
#include "LoadingRingWidget.h"
#include "AlertWidget.h"
#include "ConfirmPopupWidget.h"
#include "HostSettingsWidget.h"

class LoginWidget
{
public:
	LoginWidget(Hosting& hosting, HostSettingsWidget& hostSettingsWidget);
	void render(bool& showLogin);
	void renderPersonal(float width, bool& showLogin);
	void render3rd(float width);
	void renderCreateAccount(float width);

private:
	void renderLoginTooltip();
	void attemptLogin3rd(bool& showLogin);
	void attemptLoginPersonal(bool& showLogin);
	Hosting& _hosting;
	thread _loginThread;
	bool _isLoginLocked;
	char _email[128] = "";
	char _password[128] = "";
	char _2fa[128] = "";
	ParsecSession::AuthResult _auth;
	bool _showCancelButton = false;
	bool _sessionCancelled = false;
	string _sessionError;
	int _sessionStatus;
	bool _showError = false;
	HostSettingsWidget& _hostSettingsWidget;
};

