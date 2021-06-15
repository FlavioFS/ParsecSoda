#include "GamepadsWidget.h"

GamepadsWidget::GamepadsWidget(Hosting& hosting)
    : _hosting(hosting), _gamepads(hosting.getGamepads())
{
}

bool GamepadsWidget::render()
{
    AppStyle::pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2(800, 900));
    ImGui::Begin("Gamepads", (bool*)0);
    AppStyle::pushInput();
    
    static ImVec2 size;
    size = ImGui::GetContentRegionAvail();

    static float indentDistance;

    static vector<Gamepad>::iterator gi;
    gi = _gamepads.begin();
    static int index;
    index = 0;

    for (; gi != _gamepads.end(); ++gi)
    {
        static uint32_t userID;
        userID = (*gi).owner.guest.userID;
        

        ImGui::BeginChild(
            (string("##Gamepad " ) + to_string(index)).c_str(),
            ImVec2(size.x, 50)
        );

        static ImVec2 cursor;
        cursor = ImGui::GetCursorPos();
        
        if (IconButton::render(AppIcons::back, AppColors::primary))
        {
            (*gi).clearOwner();
        }
        TitleTooltipWidget::render("Strip gamepad", "Unlink current user from this gamepad.");

        ImGui::SameLine();

        if (ToggleIconButtonWidget::render(AppIcons::padon, AppIcons::padoff, (*gi).isConnected()))
        {
            if ((*gi).isConnected())
                (*gi).disconnect();
            else
                (*gi).connect();
        }
        TitleTooltipWidget::render("Disconnect gamepad", "\"Physically\" disconnect gamepad from PC (at O.S. level).");

        ImGui::SameLine();
        
        
        ImGui::BeginGroup();
        AppStyle::pushLabel();
        if ((*gi).owner.guest.isValid())  ImGui::TextWrapped("(# %d)\t", (*gi).owner.guest.userID);
        else                              ImGui::TextWrapped("    ");
        AppStyle::pop();

        ImGui::BeginChild(
            (string("##name ") + to_string(index)).c_str(),
            ImVec2(size.x - 160.0f, 20.0f)
        );
        AppFonts::pushInput();
        AppColors::pushPrimary();
        ImGui::TextWrapped((*gi).owner.guest.name.c_str());
        AppColors::pop();
        AppFonts::pop();
        ImGui::EndChild();
        ImGui::EndGroup();

        ImGui::SameLine();
        
        static int padIndices[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::PushID(100 * (index + 1));
        ImGui::SetNextItemWidth(50);
        padIndices[index] = (*gi).owner.deviceID;

        if (ImGui::Combo("", &padIndices[index], " 1 \0 2 \0 3 \0 4 \0 5 \0 6 \0 7 \0 8 \0\0", 8))
        {
            if (padIndices[index] >= 0 && padIndices[index] < 8)
            {
                (*gi).owner.deviceID = padIndices[index];
            }
        }
        ImGui::PopID();
        TitleTooltipWidget::render("Device index", "A guest may have multiple gamepads in the same machine.");
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::SetCursorPos(cursor);

        ImGui::InvisibleButton(
            (string("##guest button ") + to_string(index)).c_str(),
            ImVec2(size.x, 50.0f)
        );
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Guest"))
            {
                if (payload->DataSize == sizeof(int))
                {
                    int guestIndex = *(const int*)payload->Data;
                    if (guestIndex >= 0 && guestIndex < _hosting.getGuestList().size())
                    {
                        (*gi).owner.guest.copy(_hosting.getGuestList()[guestIndex]);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::EndChild();

        index++;
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    indentDistance = 0.5f * size.x - 40.0f;
    ImGui::Indent(indentDistance);
    if (ToggleIconButtonWidget::render(
        AppIcons::lock, AppIcons::unlock, _hosting.isGamepadLock(),
        AppColors::error, AppColors::alert, ImVec2(80, 80)
    ))
    {
        _hosting.toggleGamepadLock();
    }
    if (_hosting.isGamepadLock())   TitleTooltipWidget::render("Unlock guest inputs", "Guests will be able to control gamepads again.");
    else                            TitleTooltipWidget::render("Lock guest inputs", "Guest inputs will be locked out of gamepads.");
    ImGui::Unindent(indentDistance);


    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return true;
}