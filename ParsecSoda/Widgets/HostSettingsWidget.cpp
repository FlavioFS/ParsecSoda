#include "HostSettingsWidget.h"

HostSettingsWidget::HostSettingsWidget(Hosting& hosting)
    : _hosting(hosting)
{
    ParsecHostConfig cfg = hosting.getHostConfig();
    try
    {
        strcpy_s(_roomName, cfg.name);
        strcpy_s(_gameID, cfg.gameID);
        strcpy_s(_secret, cfg.secret);
    }
    catch (const std::exception&)
    {
        try
        {
            strcpy_s(_roomName, "");
            strcpy_s(_gameID, "");
            strcpy_s(_secret, "");
        } catch (const std::exception&) {}
    }
    _publicGame = cfg.publicGame;
    _maxGuests = cfg.maxGuests;
}

bool HostSettingsWidget::render()
{
    static float indentSize = 0;

    AppStyle::pushTitle();

    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 520), ImVec2(600, 900));
    ImGui::Begin("Host Settings", (bool*)0);
    AppStyle::pushLabel();

    static ImVec2 size;
    static ImVec2 pos;
    size = ImGui::GetContentRegionAvail();
    pos = ImGui::GetWindowPos();

    ImGui::Text("Room name");
    AppStyle::pushInput();
    ImGui::InputTextMultiline(" ", _roomName, HOST_NAME_LEN, ImVec2(size.x, 50));
    AppStyle::pop();

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Game ID");
    ImGui::SetNextItemWidth(size.x);
    AppStyle::pushInput();
    ImGui::InputText("  ", _gameID, GAME_ID_LEN);
    AppStyle::pop();

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Room link");
    ImGui::SetNextItemWidth(size.x);
    AppStyle::pushInput();
    ImGui::InputText("   ", _secret, LINK_COMPATIBLE_SECRET_SIZE);
    AppStyle::pop();

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SetNextItemWidth(size.x);
    ImGui::Text("Guest slots");
    ImGui::SetNextItemWidth(size.x);
    AppStyle::pushInput();
    ImGui::SliderInt("    ", &_maxGuests, 0, 64, "Max guests: %d/64");
    AppStyle::pop();

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SetNextItemWidth(size.x);
    ImGui::Text("Public room");
    ImGui::SetNextItemWidth(size.x);
    indentSize = 17.0f;
    ImGui::Indent(indentSize);
    if (ToggleIconButtonWidget::render(AppIcons::yes, AppIcons::no, _publicGame))
    {
        _publicGame = !_publicGame;
    }
    ImGui::Unindent(indentSize);
    if (_hosting.isRunning() && isDirty())
    {
        ImGui::SameLine(0.0f, size.x - 130.0f);

        if (IconButton::render(AppIcons::refresh))
        {
            _hosting.setHostConfig(_roomName, _gameID, _maxGuests, _publicGame, _secret);
            _hosting.applyHostConfig();
        }
        if (ImGui::IsItemHovered())
        {
            TooltipWidget::render("Update room settings");
        }
    }

    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    static bool showPopup = false;
    static ImVec2 popupPos = pos;
    static string popupTitle = "";
    popupTitle = (_hosting.isRunning() ? "Stop hosting?" : "Start hosting?");

    indentSize = 0.5f * size.x - 45.0f;
    ImGui::Indent(indentSize);
    if (ToggleIconButtonWidget::render(
        AppIcons::stop, AppIcons::play, _hosting.isRunning(),
        AppColors::error, AppColors::alert, ImVec2(90, 90)
    ))
    {
        showPopup = true;
        ImGui::OpenPopup(popupTitle.c_str());
        popupPos.x = ImGui::GetMousePos().x - 150;
        popupPos.y = ImGui::GetMousePos().y - 100;
    }
    ImGui::Unindent(indentSize);

    if (ImGui::IsItemHovered())
    {
        TooltipWidget::render(popupTitle.c_str());
    }

    // ================================================================================

    ImVec2 windowPos = ImGui::GetWindowPos();
    ImGui::SetNextWindowPos(popupPos);

    if (ConfirmPopupWidget::render(popupTitle.c_str(), showPopup))
    {
        // Was clicked and is already running (must stop)
        if (_hosting.isRunning())
        {
            _hosting.stopHosting();
        }

        // Was clicked and is not running (must start)
        else
        {
            _hosting.setHostConfig(_roomName, _gameID, _maxGuests, _publicGame, _secret);
            _hosting.applyHostConfig();
            _hosting.startHosting();
        }
    }

    // ================================================================================

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return true;
}

bool HostSettingsWidget::isDirty()
{
    ParsecHostConfig cfg = _hosting.getHostConfig();

    if ( 
        _publicGame != cfg.publicGame ||
        _maxGuests != cfg.maxGuests ||
        (strcmp(_roomName, cfg.name) != 0) ||
        (strcmp(_gameID, cfg.gameID) != 0) ||
        (strcmp(_secret, cfg.secret) != 0)
    ) return true;

    return false;
}
