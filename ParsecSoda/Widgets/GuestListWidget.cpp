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
        if (ImGui::BeginTabItem("Banned"))
        {
            renderBannedGuests();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Allowed"))
        {
            renderAllowedGuests();
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
    static int guestId = 0;
    guestId = 0;
    static ImVec2 cursor;
    static vector<Guest>::iterator it;
    it = _guests.begin();

    // Guests
    static bool showBanPopup = false;
    static string banPopupTitle = "";
    for (; it != _guests.end(); ++it)
    {
        if (IconButton::render(AppIcons::block, AppColors::primary, ImVec2(30, 30)))
        {
            showBanPopup = true;
            banPopupTitle = string("Ban ") + (*it).name + "?";

            ImGui::OpenPopup(banPopupTitle.c_str());
        }
        TitleTooltipWidget::render(
            "Ban user",
            (string("Press to ban ") + (*it).name + "").c_str()
        );

        if (ConfirmPopupWidget::render(
            banPopupTitle.c_str(),
            showBanPopup
        ))
        {
            _hosting.sendHostMessage((
                string("!ban ") + to_string((*it).userID)
            ).c_str());
        }

        ImGui::SameLine();

        cursor = ImGui::GetCursorPos();
        ImGui::BeginGroup();
        AppStyle::pushLabel();
        ImGui::TextWrapped("(# %d)\t", (*it).userID);
        AppStyle::pop();
        AppStyle::pushInput();
        ImGui::TextWrapped((*it).name.c_str());
        AppStyle::pop();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::EndGroup();
        ImGui::SetCursorPos(cursor);
        ImGui::Button((string("##") + to_string(guestId + 1)).c_str(), ImVec2(size.x - 45, 40));

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("Guest", &guestId, sizeof(int));

            AppFonts::pushInput();
            AppColors::pushPrimary();
            ImGui::Text("%s", (*it).name.c_str());
            AppColors::pop();
            AppFonts::pop();

            AppStyle::pushLabel();
            ImGui::Text("Drop into a Gamepad to assign.");
            AppStyle::pop();

            ImGui::EndDragDropSource();
        }

        guestId++;
    }
}

void GuestListWidget::renderBannedGuests()
{
    static bool showUnbanPopup = false;
    static string popupTitle = "";
    static size_t i, popupIndex;
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
            popupTitle = string("Unban ") + name + "?" + "##Unban " + to_string(userID);
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
    }
}


void GuestListWidget::renderAllowedGuests()
{
    static bool showBanPopup = false;
    static string popupTitle = "";
    static size_t i, popupIndex;
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
            popupTitle = string("Ban ") + name + "?" + "##Ban " + to_string(userID);

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
    }
}