#include "GuestListWidget.h"

GuestListWidget::GuestListWidget(Hosting& hosting)
    : _hosting(hosting), _guests(hosting.getGuestList())
{
}

bool GuestListWidget::render()
{
    AppStyle::pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 400), ImVec2(800, 900));
    ImGui::Begin("Guests", (bool*)0);
    AppStyle::pushInput();

    ImVec2 size = ImGui::GetContentRegionAvail();

    static vector<Guest>::iterator it;
    it = _guests.begin();
    ImGui::BeginChild("Guest List", ImVec2(size.x, size.y));

    // Host
    if (_hosting.getHost().isValid())
    {
        AppStyle::pushLabel();
        ImGui::TextWrapped("(# %d)\t", _hosting.getHost().userID);
        AppStyle::pop();
        ImGui::TextWrapped(_hosting.getHost().name.c_str());
    }
    else
    {
        AppStyle::pushLabel();
        ImGui::TextWrapped("(# ???)\t");
        AppStyle::pop();
        ImGui::TextWrapped("Unknown Host");
    }

    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    static int guestId = 0;
    guestId = 0;
    static ImVec2 cursor;

    // Guests
    for (; it != _guests.end(); ++it)
    {
        

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
        ImGui::Button( (string("##") + to_string(guestId + 1)).c_str(), ImVec2(size.x, 40));

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("Guest", &guestId, sizeof(int));

            AppStyle::pushAlert();
            ImGui::Text("%s", (*it).name.c_str());
            AppStyle::pop();
            AppStyle::pushLabel();
            ImGui::Text("Drop into a Gamepad to assign.");
            AppStyle::pop();

            ImGui::EndDragDropSource();
        }

        guestId++;
    }
    ImGui::EndChild();

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return true;
}