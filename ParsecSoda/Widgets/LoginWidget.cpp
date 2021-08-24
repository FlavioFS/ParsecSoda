#include "LoginWidget.h"

LoginWidget::LoginWidget(Hosting& hosting)
	: _hosting(hosting), _isLoginLocked(false)
{
}

void LoginWidget::render(bool& isValidSession)
{
	if (isValidSession)
		return;

    AppStyle::pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(352, 585), ImVec2(352, 585));
	ImGui::Begin(
        "Login", (bool*)0,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar
    );
    AppStyle::pop();

    static ImVec2 res, size;
    static float w;
    res = ImGui::GetMainViewport()->Size;
    size = ImGui::GetWindowSize();
    w = size.x - 40.0f;

    ImGui::Dummy(ImVec2(0, 20));

    ImGui::SetCursorPosX((size.x - 210.0f) * 0.5f);
    AppFonts::pushSugoiDekai();
    static ImVec4 loginTitleColor = ImVec4(0.00f, 0.96f, 0.68f, 1.00f);
    AppColors::pushColor(AppColors::primary);
    ImGui::Text("Parsec Soda");
    AppColors::pop();
    AppFonts::pop();
 
    ImGui::Dummy(ImVec2(0, 10));

    ImGui::SetCursorPosX((size.x - 125.0f) * 0.5f);
    ImGui::Image(AppIcons::logo, ImVec2(125, 125));

    ImGui::Dummy(ImVec2(0, 30));


    ImGui::SetWindowPos(ImVec2((res.x - size.x) * 0.5f, (res.y - size.y) * 0.5f));

    AppStyle::pushLabel();

#if USE_PARSEC_PERSONAL_API
    renderPersonal(w);
#else
    if (_auth.success && !_sessionCancelled)
    {
        render3rd(w);
    }
#endif

    if (_showError)
    {
        ImGui::OpenPopup("Login Failed");
        _showError = false;
    }

    if (AlertWidget::render("Login Failed", _sessionError.c_str()))
    {
        _showError = false;
    }

    if (!_isLoginLocked)
    {
        ImGui::Dummy(ImVec2(0, 30));

        AppColors::pushInput();
        AppFonts::pushTitle();
        ImGui::PushStyleColor(ImGuiCol_Button, AppColors::secondary);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.12f, 0.59f, 0.92f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, AppColors::primary);
        if (ImGui::Button(
            "Log in##Login Button",
            ImVec2(w, 50)
        ))
        {
            attemptLogin3rd();
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        AppFonts::pop();
        AppColors::pop();
        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        renderCreateAccount(size.x);
    }
    else
    {
        ImGui::SetCursorPosX((size.x - 100.0f) * 0.5f);
        LoadingRingWidget::render();

        ImGui::Dummy(ImVec2(0, 120));

        if (_showCancelButton)
        {
            AppColors::pushInput();
            AppFonts::pushTitle();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.23f, 0.23f, 0.23f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.30f, 0.30f, 0.30f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.40f, 0.40f, 0.40f, 1.00f));
            if (ImGui::Button(
                "Cancel##Login Cancel",
                ImVec2(w, 50)
            ))
            {
                _sessionCancelled = true;
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            AppFonts::pop();
            AppColors::pop();
            if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
    }

    AppStyle::pop();

	ImGui::End();
}

void LoginWidget::renderPersonal(float width)
{
    ImGui::Text("E-mail");
    AppStyle::pushInput();
    ImGui::SetNextItemWidth(width);
    ImGui::InputText("##Login e-mail", _email, 128);
    AppStyle::pop();
    renderLoginTooltip();

    ImGui::Dummy(ImVec2(0, 5));

    ImGui::Text("Password");
    ImGui::SetNextItemWidth(width);
    AppStyle::pushInput();
    if (ImGui::InputText("##Login password", _password, 128, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue))
    {
        attemptLoginPersonal();
    }
    AppStyle::pop();
    renderLoginTooltip();

    ImGui::Dummy(ImVec2(0, 5));

    ImGui::Text("2FA");
    AppStyle::pushInput();
    ImGui::SetNextItemWidth(width);
    ImGui::InputText("##2fa", _2fa, 128);
    AppStyle::pop();
    TitleTooltipWidget::render(
        "Two-Factor Authentication (2FA)",
        "Only fill this if you have 2FA enabled at your account.\nLeave blank otherwise."
    );
}

void LoginWidget::render3rd(float width)
{

    ImGui::Text("Auth Code");
    AppColors::pushPrimary();
    AppFonts::pushSugoiDekai();
    ImGui::SetNextItemWidth(width);
    ImGui::InputText("##Login auth", _auth.userCode, 16, ImGuiInputTextFlags_ReadOnly);
    AppFonts::pop();
    AppColors::pop();
    TitleTooltipWidget::render(
        "Authentication Code",
        "Copy and Paste this code to the link in your browser window\n\nIf a browser window does not show automatically, go to:\nhttps://parsec.app/activate/"
    );
}

void LoginWidget::renderCreateAccount(float width)
{
    AppFonts::pushInput();
    AppColors::pushPrimary();
    ImGui::SetCursorPosX(width - 166.0f);
    if (ImGui::Button("Create new account"))
    {
        ShellExecute(0, 0, L"https://parsec.app/activate/", 0, 0, SW_SHOW);
    }
    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    AppColors::pop();
    AppFonts::pop();
}

void LoginWidget::renderLoginTooltip()
{
    TitleTooltipWidget::render(
        "Parsec Account",
        (
            string () +
            "Login with your Parsec account.\n" +
            "* Parsec Soda does not collect any data from you."
        ).c_str()
    );
}

void LoginWidget::attemptLoginPersonal()
{
    if (!_isLoginLocked)
    {
        _isLoginLocked = true;
        LoadingRingWidget::render(true);
        _loginThread = thread([&]() {
            _hosting.getSession().fetchSession(_email, _password, _2fa);

            if (!_hosting.getSession().isValid())
            {
                _sessionError = _hosting.getSession().getSessionError();
                _sessionStatus = _hosting.getSession().getSessionStatus();
                _showError = true;
            }

            _hosting.fetchAccountData();
            _isLoginLocked = false;
            _loginThread.detach();
        });
    }
}

void LoginWidget::attemptLogin3rd()
{
    if (!_isLoginLocked)
    {
        _sessionCancelled = false;
        _showCancelButton = false;
        _isLoginLocked = true;
        LoadingRingWidget::render(true);
        _loginThread = thread([&]() {
            _auth = _hosting.getSession().authenticate();

            if (_auth.success)
            {
                string uri(_auth.verificationUri);
                wstring wuri(&uri[0], &uri[uri.size()]);
                ShellExecute(0, 0, wuri.c_str(), 0, 0, SW_SHOW);
            }

            // Minimum auth cooldown
            Sleep((_auth.interval+1) * 1000);
            ParsecSession::SessionStatus status;

            _showCancelButton = true;

            bool done = false;
            while (!done && !_sessionCancelled)
            {
                status = _hosting.getSession().pollSession(_auth);
                switch (status)
                {
                case ParsecSession::SessionStatus::PENDING:
                    Sleep(1500);
                    break;
                case ParsecSession::SessionStatus::APPROVED:
                    done = true;
                    break;
                case ParsecSession::SessionStatus::INVALID:
                default:
                    done = true;
                    break;
                }
            }

            _hosting.fetchAccountData();
            _isLoginLocked = false;
            _loginThread.detach();
        });
    }
}
