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
#include "../globals/AppColors.h"

class LoginWidget
{
public:
	LoginWidget(Hosting& hosting);
	void render(bool& isValidSession);
	void renderPersonal(float width);
	void render3rd(float width);
	void renderCreateAccount(float width);

private:
	void renderLoginTooltip();
	Hosting& _hosting;
	thread _loginThread;
	bool _isLoginLocked;
	void attemptLogin3rd();
	void attemptLoginPersonal();
	char _email[128] = "";
	char _password[128] = "";
	char _2fa[128] = "";
	ParsecSession::AuthResult _auth;
	bool _showCancelButton = false;
	bool _sessionCancelled = false;
	string _sessionError;
	int _sessionStatus;
	bool _showError = false;
};

