#include "GuestListWidget.h"

GuestListWidget::GuestListWidget(Hosting& hosting)
    : _hosting(hosting), _banList(hosting.getBanList()), _historyList(hosting.getGuestHistory()), _metricsHistory(hosting.getGuestMetricsHistory())
{
}

bool GuestListWidget::render()
{
    AppStyle::pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(800, 900));
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

    static GuestData guestData = GuestData(guest.name, guest.userID);

    Action enqueueKickAction = [&]() { _actionQueue.add([&]() {sendHostMessage("!kick", guest.userID); }); };
    renderPopupButton("Kick", "!kick", guestData, showKickPopup, kickPopupTitle, AppIcons::kick, enqueueKickAction);
    
    ImGui::SameLine();
    
    Action enqueueBanAction = [&]() { _actionQueue.add([&]() {sendHostMessage("!ban", guest.userID); }); };
    renderPopupButton("Ban", "!ban", guestData, showBanPopup, banPopupTitle, AppIcons::block, enqueueBanAction);


    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 1));
    ImGui::SameLine();

    // ===================
    // Name and userID
    // ===================
    cursor = ImGui::GetCursorPos();
    ImGui::BeginGroup();
    AppStyle::pushLabel();
    ImGui::TextWrapped("(# %d)\t", guest.userID);
    AppStyle::pop();
    AppStyle::pushInput();
    ImGui::TextWrapped(guest.name.c_str());
    AppStyle::pop();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::EndGroup();
    ImGui::SetCursorPos(cursor);
    ImGui::Button((string("##") + to_string(index + 1)).c_str(), ImVec2(size.x - 45, 40));

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

    ImGui::Dummy(ImVec2(10.0f, 0.0f));
    ImGui::SameLine();

    // ===================
    // Latency
    // ===================
    static ImVec4 latencyColor;
    latencyColor = getLatencyColor(guest.metrics.getLatency());
    AppStyle::pushInput();
    AppColors::pushColor(latencyColor);
    ImGui::Text("%.0f ms", guest.metrics.getLatency());
    AppColors::pop();
    AppStyle::pop();

    ImGui::PopStyleVar();
}

void GuestListWidget::renderOneBannedGuest(GuestData& guest, size_t index)
{
    static bool showUnbanPopup = false;
    static string popupTitle = "";
    static size_t popupIndex;

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
}

void GuestListWidget::renderOneHistoryGuest(GuestData& guest, size_t index)
{
    static bool showBanPopup = false;
    static string popupTitle = "";
    static size_t popupIndex;

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
    if (!filterTextStr.empty()) return true;
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

void GuestListWidget::renderPopupButton(const string title, const string command, const GuestData guest, bool& showPopup, string& popupTitle, Icon btnIcon, Action action)
{
    IconButton::render(btnIcon, AppColors::primary, ImVec2(30, 30));
    if (ImGui::IsItemActive())
    {
        showPopup = true;
        popupTitle = title + " " + guest.name + "?" + "##Online " + title + to_string(guest.userID);
        ImGui::OpenPopup(popupTitle.c_str());
    }
    TitleTooltipWidget::render(
        (title + " user").c_str(),
        (string("Press to ") + title + " " + guest.name).c_str()
    );
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
    _metricsHistory.findSafe(guest.userID, [&](CircularList<GuestMetrics>& guestMetricsLog) {
        AbstractTooltipWidget::render(guest.name.c_str(), [&]() {
            GuestMetricsHistoryGraphWidget::render(guest, guestMetricsLog);
        });
    });
}
