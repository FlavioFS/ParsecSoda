#include "HostSettingsWidget.h"

HostSettingsWidget::HostSettingsWidget(AppIcons& icons, Hosting& hosting)
    : _icons(icons), _hosting(hosting)
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

    _publicRoomBtn = ToggleTexButtonWidget (icons.yes, icons.no, false);
    _playBtn = ToggleTexButtonWidget(icons.stop, icons.play, false, ImVec2(120, 120));
}

bool HostSettingsWidget::render(AppStyle& style)
{
    style.pushTitle();

    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 700), ImVec2(600, 900));
    ImGui::Begin("Host Settings", (bool*)0);
    style.pushLabel();

    static ImVec2 size;
    static ImVec2 pos;
    size = ImGui::GetContentRegionAvail();
    pos = ImGui::GetWindowPos();

    ImGui::Text("Room name");
    style.pushInput();
    ImGui::InputTextMultiline(" ", _roomName, HOST_NAME_LEN, ImVec2(size.x, 50));
    style.pop();

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Game ID");
    ImGui::SetNextItemWidth(size.x);
    style.pushInput();
    ImGui::InputText("  ", _gameID, GAME_ID_LEN);
    style.pop();

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Room link");
    ImGui::SetNextItemWidth(size.x);
    style.pushInput();
    ImGui::InputText("   ", _secret, LINK_COMPATIBLE_SECRET_SIZE);
    style.pop();

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SetNextItemWidth(size.x);
    ImGui::Text("Guest slots");
    ImGui::SetNextItemWidth(size.x);
    style.pushInput();
    ImGui::SliderInt("    ", &_maxGuests, 0, 64, "Max guests: %d/64");
    style.pop();

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SetNextItemWidth(size.x);
    ImGui::Text("Public room");
    ImGui::SetNextItemWidth(size.x);
    ImGui::Indent(17.0f);
    _publicRoomBtn.render(_publicGame);
    if (_hosting.isRunning() && isDirty())
    {
        ImGui::SameLine(0.0f, size.x - 130.0f);

        if (ImGui::ImageButton(_icons.refresh, DEFAULT_BUTTON_SIZE))
        {
            _hosting.setHostConfig(_roomName, _gameID, _maxGuests, _publicGame, _secret);
            _hosting.applyHostConfig();
        }
        if (ImGui::IsItemHovered())
        {
            TooltipWidget::render("Update room settings", style);
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Indent((size.x - 165.0) / 2);
    _playBtn.setOn(_hosting.isRunning());
    static bool showPopup = false;
    static ImVec2 popupPos = pos;
    static string popupTitle = "";
    popupTitle = (_hosting.isRunning() ? "Stop hosting?" : "Start hosting?");

    if (_playBtn.render())
    {
        showPopup = true;
        ImGui::OpenPopup(popupTitle.c_str());
        popupPos.x = ImGui::GetMousePos().x - 150;
        popupPos.y = ImGui::GetMousePos().y - 100;
    }

    if (ImGui::IsItemHovered())
    {
        TooltipWidget::render(popupTitle.c_str(), style);
    }

    // ================================================================================
    
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImGui::SetNextWindowPos(popupPos);
    
    if(ConfirmPopupWidget::render(popupTitle.c_str(), showPopup, _icons, style))
    {
        // Was clicked and is already running (must stop)
        if (_hosting.isRunning())
        {
            _hosting.stopHosting();
            _playBtn.setOn(false);
        }

        // Was clicked and is not running (must start)
        else
        {
            _hosting.setHostConfig(_roomName, _gameID, _maxGuests, _publicGame, _secret);
            _hosting.applyHostConfig();
            _hosting.startHosting();
            _playBtn.setOn(true);
        }
    }

    // ================================================================================












    style.pop();
    ImGui::End();
    style.pop();

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
