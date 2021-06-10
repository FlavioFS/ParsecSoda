#include "HostSettingsWidget.h"

HostSettingsWidget::HostSettingsWidget(AppIcons* icons, Hosting * hosting)
    : _icons(icons), _hosting(hosting)
{
    ParsecHostConfig cfg = hosting->getHostConfig();
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

    _publicRoomBtn = ToggleTexButtonWidget (icons->yes().img(), icons->no().img(), false);
    _playBtn = ToggleTexButtonWidget(icons->stop().img(), icons->play().img(), false, ImVec2(120, 120));
}

bool HostSettingsWidget::render()
{
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 700), ImVec2(600, 900));
    ImGui::Begin("Host Settings", (bool*)0);

    float width = ImGui::GetContentRegionAvail().x;

    ImGui::Text("Room name");
    ImGui::InputTextMultiline(" ", _roomName, HOST_NAME_LEN, ImVec2(width, 50));

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Game ID");
    ImGui::SetNextItemWidth(width);
    ImGui::InputText("  ", _gameID, GAME_ID_LEN);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Room link");
    ImGui::SetNextItemWidth(width);
    ImGui::InputText("   ", _secret, LINK_COMPATIBLE_SECRET_SIZE);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SetNextItemWidth(width);
    ImGui::Text("Guest slots");
    ImGui::SetNextItemWidth(width);
    ImGui::SliderInt("    ", &_maxGuests, 0, 64, "Max guests: %d/64");

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SetNextItemWidth(width);
    ImGui::Text("Public room");
    ImGui::SetNextItemWidth(width);
    ImGui::Indent(25.0f);
    _publicRoomBtn.render(_publicGame);
    if (_hosting->isRunning() && isDirty())
    {
        ImGui::SameLine(0.0f, width - 150.0f);

        if (ImGui::ImageButton(_icons->refresh().img(), DEFAULT_BUTTON_SIZE))
        {
            _hosting->setHostConfig(_roomName, _gameID, _maxGuests, _publicGame, _secret);
            _hosting->applyHostConfig();
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Update room settings");
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Indent((width - 180.0) / 2);
    _playBtn.setOn(_hosting->isRunning());
    if (_playBtn.render())
    {
        // Was clicked and is already running (must stop)
        if (_hosting->isRunning())
        {
            _hosting->stopHosting();
            _playBtn.setOn(false);
        }

        // Was clicked and is not running (must start)
        else
        {
            _hosting->setHostConfig(_roomName, _gameID, _maxGuests, _publicGame, _secret);
            _hosting->applyHostConfig();
            _hosting->startHosting();
            _playBtn.setOn(true);
        }
    }

    // Does not matter if clicked or not
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(_hosting->isRunning() ? "Stop Hosting" : "Start hosting");
    }

    ImGui::End();

    return true;
}

bool HostSettingsWidget::isDirty()
{
    ParsecHostConfig cfg = _hosting->getHostConfig();

    if ( 
        _publicGame != cfg.publicGame ||
        _maxGuests != cfg.maxGuests ||
        (strcmp(_roomName, cfg.name) != 0) ||
        (strcmp(_gameID, cfg.gameID) != 0) ||
        (strcmp(_secret, cfg.secret) != 0)
    ) return true;

    return false;
}
