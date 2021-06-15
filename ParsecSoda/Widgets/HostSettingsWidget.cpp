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
    static ImVec2 dummySize = ImVec2(0.0f, 5.0f);
    static ImVec2 cursor;

    AppStyle::pushTitle();

    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 520), ImVec2(600, 900));
    ImGui::Begin("Host Settings", (bool*)0);
    AppStyle::pushLabel();

    static ImVec2 size;
    static ImVec2 pos;
    size = ImGui::GetContentRegionAvail();
    pos = ImGui::GetWindowPos();

    
    ImGui::Dummy(ImVec2(0, 10.0f));

    ImGui::Text("Room name");
    AppStyle::pushInput();
    ImGui::InputTextMultiline(" ", _roomName, HOST_NAME_LEN, ImVec2(size.x, 50));
    TitleTooltipWidget::render("Room Title", "The text displayed below thumbnails in Arcade list.");
    AppStyle::pop();

    if (!_hosting.isRunning() && isDirty())
    {
        cursor = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(size.x - 30.0f, 35.0f));

        if (IconButton::render(AppIcons::submit, AppColors::primary))
        {
            _hosting.setHostConfig(_roomName, _gameID, _maxGuests, _publicGame, _secret);
            _hosting.applyHostConfig();
        }
        TitleTooltipWidget::render("Update Room Settings", "The room will be instantly updated with your new settings.");
        
        ImGui::SetCursorPos(cursor);
    }

    ImGui::Dummy(dummySize);

    ImGui::Text("Game ID");
    ImGui::SetNextItemWidth(size.x);
    AppStyle::pushInput();
    ImGui::InputText("  ", _gameID, GAME_ID_LEN);
    TitleTooltipWidget::render("Thumbnail ID", "Sets thumbnail, but you need to know beforehand\nthe specific hash value for your game.");
    AppStyle::pop();

    ImGui::Dummy(dummySize);

    ImGui::Text("Room link");
    ImGui::SetNextItemWidth(size.x);
    AppStyle::pushInput();
    ImGui::InputText("   ", _secret, LINK_COMPATIBLE_SECRET_SIZE);
    TitleTooltipWidget::render("Room Secret", "Generates the share link that lets people\njoin your room anytime.");
    AppStyle::pop();

    ImGui::Dummy(dummySize);

    cursor = ImGui::GetCursorPos();

    ImGui::BeginChild("##Public room child", ImVec2(120.0f, 75.0f));
    ImGui::Text("Guest slots");
    if (IntRangeWidget::render("guest count", _maxGuests, 0, 64, 0.05f))
    {
        TitleTooltipWidget::render("Room Slots", "How many guests do you want in this room?");
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::SetCursorPos(ImVec2(cursor.x + size.x - 90.0f, cursor.y));
    BoolButtonWidget::render("Public room", _publicGame);

    if (_publicGame)    TitleTooltipWidget::render("Public Game", "Anyone can enter this room.");
    else                TitleTooltipWidget::render("Private Game", "All guests must use the secret link to enter this room.");

    ImGui::Dummy(dummySize);

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

    TooltipWidget::render(popupTitle.c_str());

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
