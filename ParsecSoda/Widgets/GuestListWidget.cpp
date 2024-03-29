#include "GuestListWidget.h"

GuestListWidget::GuestListWidget(Hosting& hosting)
    : _hosting(hosting),
    _banList(hosting.getBanList()), _historyList(hosting.getGuestHistory()),
    _mutedGuests(hosting.getMutedGuests()), _metricsHistory(hosting.getGuestMetricsHistory())
{
}

bool GuestListWidget::render()
{
    AppStyle::pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(250, 300), ImVec2(800, 900));
    ImGui::Begin("Guests", (bool*)0);
    AppStyle::pushInput();

    ImVec2 size = ImGui::GetContentRegionAvail();

    ImGui::BeginChild("Guest List", ImVec2(size.x, size.y));

    AppStyle::pushLabel();
    ImGui::Text("Filter Guests");
    AppStyle::pop();
    ImGui::SetNextItemWidth(size.x);
    ImGui::InputText("### Guest List Filter", _filterText, 256);
    TitleTooltipWidget::render("Filter Guests", "Type user Name or ID to filter.");

    ImGui::Dummy(ImVec2(0, 5));

    if (ImGui::BeginTabBar("Guest Tabs", ImGuiTabBarFlags_None))
    {
        AppFonts::pushInput();
        AppColors::pushTitle();
        if (ImGui::BeginTabItem("Online"))
        {
            renderOnlineGuests();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Blocked"))
        {
            renderBannedGuests();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("History"))
        {
            renderHistoryGuests();
            ImGui::EndTabItem();
        }
        AppColors::pop();
        AppFonts::pop();
        ImGui::EndTabBar();
    }
    
    ImGui::EndChild();

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return true;
}

void GuestListWidget::renderOnlineGuests()
{
    renderAnyGuestList(_hosting.getGuestList(), [&](Guest& guest, size_t index) { renderOneOnlineGuest(guest, index); });
}

void GuestListWidget::renderBannedGuests()
{
    renderAnyGuestDataList(_banList, [&](GuestData& guest, size_t index) { renderOneBannedGuest(guest, index); });
}


void GuestListWidget::renderHistoryGuests()
{
    renderAnyGuestDataList(_historyList, [&](GuestData& guest, size_t index) { renderOneHistoryGuest(guest, index); });
}

void GuestListWidget::renderOneOnlineGuest(Guest& guest, size_t index)
{
    static ImVec2 size;
    size = ImGui::GetContentRegionAvail();
    static ImVec2 cursor;

    static bool showKickPopup = false;
    static string kickPopupTitle = "";
    static bool showBanPopup = false;
    static string banPopupTitle = "";

    static GuestData guestData;
    guestData = GuestData(guest.name, guest.userID);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));

    ImGui::Dummy(ImVec2(1, 5));

    ImGui::PushID(("## Online guest " + to_string(index)).c_str());

    // ===================
    // Name and userID
    // ===================
    cursor = ImGui::GetCursorPos();
    ImGui::BeginGroup();
    AppStyle::pushLabel();
    ImGui::TextWrapped("# %d\t", guest.userID);
    AppStyle::pop();
    AppStyle::pushInput();
    ImGui::TextWrapped(guest.name.c_str());
    AppStyle::pop();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::EndGroup();
    ImGui::SetCursorPos(cursor);
    ImGui::Button((string("##") + to_string(index + 1)).c_str(), ImVec2(size.x, 30));

    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("Guest", &index, sizeof(int));

        AppFonts::pushInput();
        AppColors::pushPrimary();
        ImGui::Text("%s", guest.name.c_str());
        AppColors::pop();
        AppFonts::pop();

        AppStyle::pushLabel();
        ImGui::Text("Drop into a Gamepad to assign.");
        AppStyle::pop();

        ImGui::EndDragDropSource();
    }

    static bool isMuted;
    isMuted = _mutedGuests.find(guest.userID);
    if (ToggleIconButtonWidget::render(
        AppIcons::chatOff, AppIcons::chatOn,
        isMuted, ("Mute Online Guest " + to_string(index)).c_str(),
        AppColors::red, AppColors::positive,
        ImVec2(20, 20)
    ))
    {
        if (isMuted)
        {
            _mutedGuests.pop(guest.userID);
        }
        else
        {
            _mutedGuests.add(GuestData(guest.name, guest.userID));
        }
    }
    if (isMuted)  TitleTooltipWidget::render("Muted guest", ("Guest cannot type in chat.\nPress to unmute " + guest.name + ".").c_str());
    else          TitleTooltipWidget::render("Chatting guest", ("Guest can type in chat.\nPress to mute " + guest.name + ".").c_str());

    ImGui::SameLine();

    Action enqueueKickAction = [&]() { _actionQueue.add([&]() {sendHostMessage("!kick", guest.userID); }); };
    renderPopupButton("Kick", "!kick", guestData, index, showKickPopup, kickPopupTitle, AppIcons::kick, enqueueKickAction);
    
    ImGui::SameLine();

    Action enqueueBanAction = [&]() { _actionQueue.add([&]() {sendHostMessage("!ban", guest.userID); }); };
    renderPopupButton("Ban", "!ban", guestData, index, showBanPopup, banPopupTitle, AppIcons::block, enqueueBanAction);


    ImGui::SameLine();
    ImGui::Dummy(ImVec2(5, 0));
    ImGui::SameLine();

    // ===================
    // Latency
    // ===================
    static ImVec4 latencyColor;
    static const float TEXT_HEIGHT = ImGui::CalcTextSize("ms").y, BUTTON_HEIGHT = 20;
    latencyColor = getLatencyColor(guest.metrics.getLatency());
    AppStyle::pushInput();
    AppColors::pushColor(latencyColor);
    ImGui::AlignTextToFramePadding();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 0.5f * (BUTTON_HEIGHT - TEXT_HEIGHT));
    ImGui::Text("%.0f ms", guest.metrics.getLatency());
    renderGuestMetricsTooltip(guestData);
    AppColors::pop();
    AppStyle::pop();

    ImGui::PopID();
    ImGui::PopStyleVar(2);
}

void GuestListWidget::renderOneBannedGuest(GuestData& guest, size_t index)
{
    static bool showUnbanPopup = false;
    static string popupTitle = "";
    static size_t popupIndex;

    ImGui::PushID((string("Banned Guest ") + to_string(index)).c_str());

    IconButton::render(AppIcons::userOff, AppColors::negative, ImVec2(30, 30));
    if (ImGui::IsItemActive())
    {
        popupTitle = string("Unban ") + guest.name + "?" + "##Blocked Unban " + to_string(guest.userID);
        showUnbanPopup = true;
        popupIndex = index;
        ImGui::OpenPopup(popupTitle.c_str());
    }
    TitleTooltipWidget::render(
        "Blocked user",
        (string("Press to unban ") + guest.name + "").c_str()
    );

    if (index == popupIndex)
    {
        if (ConfirmPopupWidget::render(
            popupTitle.c_str(),
            showUnbanPopup
        ))
        {
            _actionQueue.add([&]() {sendHostMessage("!unban", guest.userID); });
        }
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 1));
    ImGui::SameLine();

    ImGui::BeginGroup();
    AppStyle::pushLabel();
    ImGui::TextWrapped("(# %d)\t", guest.userID);
    AppStyle::pop();
    AppStyle::pushInput();
    ImGui::TextWrapped(guest.name.c_str());
    AppStyle::pop();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::EndGroup();
    ImGui::PopStyleVar();

    ImGui::PopID();
}

void GuestListWidget::renderOneHistoryGuest(GuestData& guest, size_t index)
{
    static bool showBanPopup = false;
    static string popupTitle = "";
    static size_t popupIndex;

    ImGui::PushID((string("History Guest ") + to_string(index)).c_str());

    IconButton::render(AppIcons::block, AppColors::primary, ImVec2(30, 30));
    if (ImGui::IsItemActive())
    {
        showBanPopup = true;
        popupTitle = string("Ban ") + guest.name + "?" + "##History Ban " + to_string(guest.userID);
        popupIndex = index;
        ImGui::OpenPopup(popupTitle.c_str());
    }

    TitleTooltipWidget::render("Allowed user", (string("Press to ban ") + guest.name).c_str());

    if (index == popupIndex)
    {
        if (ConfirmPopupWidget::render(
            popupTitle.c_str(),
            showBanPopup
        ))
        {
            _actionQueue.add([&]() {sendHostMessage("!ban", guest.userID); });
        }
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 1));
    ImGui::SameLine();

    ImGui::BeginGroup();
    AppStyle::pushLabel();
    ImGui::TextWrapped("(# %d)\t", guest.userID);
    AppStyle::pop();
    AppStyle::pushInput();
    ImGui::TextWrapped(guest.name.c_str());
    AppStyle::pop();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::EndGroup();
    ImGui::PopStyleVar();

    ImGui::PopID();
}


void GuestListWidget::renderAnyGuestList(GuestList& guestList, function<void(Guest&, size_t index)> renderOneGuest)
{
    static size_t index;

    guestList.getGuestsSafe([&](vector<Guest>& guests) {

        index = 0;
        for (size_t i = 0; i < guests.size(); ++i)
        {
            if (!isGuestFilterMatch(guests[i].name, guests[i].userID, _filterText))
            {
                continue;
            }

            if (renderOneGuest)
            {
                renderOneGuest(guests[i], index);
            }

            ++index;
        }

      });

    _actionQueue.runAll();
}

void GuestListWidget::renderAnyGuestDataList(GuestDataList& guestDataList, function<void(GuestData&, size_t index)> renderOneGuest)
{
    static size_t index;

    guestDataList.getGuests([&](vector<GuestData>& guests) {
        
        index = 0;
        for (size_t i = 0; i < guests.size(); ++i)
        {
            if (!isGuestFilterMatch(guests[i].name, guests[i].userID, _filterText))
            {
                continue;
            }

            if (renderOneGuest)
            {
                renderOneGuest(guests[i], index);
            }
        
            ++index;
        }

    });

    _actionQueue.runAll();
}

bool GuestListWidget::isGuestFilterMatch(const string name, const uint32_t userID, const string filterTextStr)
{
    if (filterTextStr.empty()) return true;
    if (Stringer::fuzzyDistance(_filterText, name) == 0) return true;
    if (Stringer::fuzzyDistance(_filterText, to_string(userID)) == 0) return true;

    return false;
}

void GuestListWidget::sendHostMessage(const string command, const uint32_t userID)
{
    _hosting.sendHostMessage((
        command + string(" ") + to_string(userID)
    ).c_str(), true);
}

ImVec4 GuestListWidget::getLatencyColor(float latency)
{
    if (latency < 80) return AppColors::positive;
    if (latency >= 150) return AppColors::negative;
    return AppColors::warning;
}

void GuestListWidget::renderPopupButton(const string title, const string command, const GuestData& guest, const size_t& index, bool& showPopup, string& popupTitle, Icon btnIcon, Action action)
{
    static string tooltipTitle, tooltipDescription, id;

    tooltipTitle = title + " user";
    tooltipDescription = "Press to " + title + " " + guest.name;
    id = title + " " + to_string(index);
    if (BadgeButtonWidget::render(btnIcon, tooltipTitle.c_str(), tooltipDescription.c_str(), id.c_str(), ImVec2(20, 20)))
    {
        showPopup = true;
        popupTitle = title + " " + guest.name + "?" + "##Online " + to_string(index)+ title + to_string(guest.userID);
        ImGui::OpenPopup(popupTitle.c_str());
    }

    if (showPopup)
    {
        if (ConfirmPopupWidget::render(
            popupTitle.c_str(),
            showPopup
        ))
        {
            if (action) action();
        }
    }
}

void GuestListWidget::renderGuestMetricsTooltip(GuestData guest)
{
    AbstractTooltipWidget::render(guest.name.c_str(), "Network Stats", [&]() {
        _metricsHistory.findSafe(guest.userID, [&](CircularMetrics& guestMetricsLog) {
            GuestMetricsHistoryGraphWidget::render(guest, guestMetricsLog);
        });
    });
}
