#include "GuestListWidget.h"

GuestListWidget::GuestListWidget(Hosting& hosting)
    : _hosting(hosting), _guests(hosting.getGuestList()),
    _banList(_hosting.getBanList()), _guestHistory(_hosting.getGuestHistory())
{
}

bool GuestListWidget::render()
{
    AppStyle::pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(800, 900));
    ImGui::Begin("Guests", (bool*)0);
    AppStyle::pushInput();

    ImVec2 size = ImGui::GetContentRegionAvail();

    static vector<Guest>::iterator it;
    it = _guests.begin();
    ImGui::BeginChild("Guest List", ImVec2(size.x, size.y));

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
    static ImVec2 size;
    size = ImGui::GetContentRegionAvail();
    static size_t popupIndex;
    static string name;
    static uint32_t userID;
    static ImVec2 cursor;

    // Guests
    static bool showBanPopup = false;
    static string banPopupTitle = "";
    for (size_t i = 0; i < _guests.size(); ++i)
    {
        name = _guests[i].name;
        userID = _guests[i].userID;

        IconButton::render(AppIcons::block, AppColors::primary, ImVec2(30, 30));
        if (ImGui::IsItemActive())
        {
            showBanPopup = true;
            banPopupTitle = string("Ban ") + name + "?" + "##Online Ban " + to_string(userID);
            popupIndex = i;
            ImGui::OpenPopup(banPopupTitle.c_str());
        }
        TitleTooltipWidget::render(
            "Ban user",
            (string("Press to ban ") + name + "").c_str()
        );

        if (i == popupIndex)
        {
            if (ConfirmPopupWidget::render(
                banPopupTitle.c_str(),
                showBanPopup
            ))
            {
                _hosting.sendHostMessage((
                    string("!ban ") + to_string(userID)
                ).c_str());
            }
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 1));
        ImGui::SameLine();

        cursor = ImGui::GetCursorPos();
        ImGui::BeginGroup();
        AppStyle::pushLabel();
        ImGui::TextWrapped("(# %d)\t", userID);
        AppStyle::pop();
        AppStyle::pushInput();
        ImGui::TextWrapped(name.c_str());
        AppStyle::pop();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::EndGroup();
        ImGui::PopStyleVar();
        ImGui::SetCursorPos(cursor);
        ImGui::Button((string("##") + to_string(i + 1)).c_str(), ImVec2(size.x - 45, 40));

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("Guest", &i, sizeof(int));

            AppFonts::pushInput();
            AppColors::pushPrimary();
            ImGui::Text("%s", name.c_str());
            AppColors::pop();
            AppFonts::pop();

            AppStyle::pushLabel();
            ImGui::Text("Drop into a Gamepad to assign.");
            AppStyle::pop();

            ImGui::EndDragDropSource();
        }
    }
}

void GuestListWidget::renderBannedGuests()
{
    static bool showUnbanPopup = false;
    static string popupTitle = "";
    static size_t popupIndex;
    static string name;
    static uint32_t userID;
    static vector<GuestData>& _bannedGuests = _banList.getGuests();

    for (size_t i = 0; i < _bannedGuests.size(); ++i)
    {
        name = _bannedGuests[i].name;
        userID = _bannedGuests[i].userID;

        IconButton::render(AppIcons::userOff, AppColors::negative, ImVec2(30, 30));
        if (ImGui::IsItemActive())
        {
            popupTitle = string("Unban ") + name + "?" + "##Blocked Unban " + to_string(userID);
            showUnbanPopup = true;
            popupIndex = i;
            ImGui::OpenPopup(popupTitle.c_str());
        }
        TitleTooltipWidget::render(
            "Blocked user",
            (string("Press to unban ") + name + "").c_str()
        );

        if (i == popupIndex)
        {
            if (ConfirmPopupWidget::render(
                popupTitle.c_str(),
                showUnbanPopup
            ))
            {
                _hosting.sendHostMessage((
                    string("!unban ") + to_string(userID)
                ).c_str());
            }
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 1));
        ImGui::SameLine();
        
        ImGui::BeginGroup();
        AppStyle::pushLabel();
        ImGui::TextWrapped("(# %d)\t", userID);
        AppStyle::pop();
        AppStyle::pushInput();
        ImGui::TextWrapped(name.c_str());
        AppStyle::pop();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::EndGroup();
        ImGui::PopStyleVar();
    }
}


void GuestListWidget::renderHistoryGuests()
{
    static bool showBanPopup = false;
    static string popupTitle = "";
    static size_t popupIndex;
    static string name;
    static uint32_t userID;

    for (size_t i = 0; i < _guestHistory.size(); ++i)
    {
        name = _guestHistory[i].name;
        userID = _guestHistory[i].userID;

        IconButton::render(AppIcons::block, AppColors::primary, ImVec2(30, 30));
        if(ImGui::IsItemActive())
        {
            showBanPopup = true;
            popupTitle = string("Ban ") + name + "?" + "##History Ban " + to_string(userID);
            popupIndex = i;
            ImGui::OpenPopup(popupTitle.c_str());
        }

        TitleTooltipWidget::render("Allowed user", (string("Press to ban ") + name).c_str());

        if (i == popupIndex)
        {
            if (ConfirmPopupWidget::render(
                popupTitle.c_str(),
                showBanPopup
            ))
            {
                _hosting.sendHostMessage((
                    string("!ban ") + to_string(userID)
                ).c_str());
            }
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 1));
        ImGui::SameLine();

        ImGui::BeginGroup();
        AppStyle::pushLabel();
        ImGui::TextWrapped("(# %d)\t", userID);
        AppStyle::pop();
        AppStyle::pushInput();
        ImGui::TextWrapped(name.c_str());
        AppStyle::pop();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::EndGroup();
        ImGui::PopStyleVar();
    }
}