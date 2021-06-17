#include "GamepadsWidget.h"

GamepadsWidget::GamepadsWidget(Hosting& hosting)
    : _hosting(hosting), _gamepads(hosting.getGamepads())
{
}

bool GamepadsWidget::render()
{
    static ImVec2 dummySize = ImVec2(0.0f, 5.0f);

    AppStyle::pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(400, 375), ImVec2(800, 900));
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

        if (ToggleIconButtonWidget::render(AppIcons::padOn, AppIcons::padOff, (*gi).isConnected()))
        {
            if ((*gi).isConnected())
                (*gi).disconnect();
            else
                (*gi).connect();
        }
        TitleTooltipWidget::render("Disconnect gamepad", "\"Physically\" disconnect gamepad from PC (at O.S. level).");

        ImGui::SameLine();
        
        static float gamepadLabelWidth;
        gamepadLabelWidth = size.x - 160.0f;
        
        ImGui::BeginChild(
            (string("##name ") + to_string(index)).c_str(),
            ImVec2(gamepadLabelWidth, 50.0f)
        );
        cursor = ImGui::GetCursorPos();

        AppStyle::pushLabel();
        if ((*gi).owner.guest.isValid())  ImGui::TextWrapped("(# %d)\t", (*gi).owner.guest.userID);
        else                              ImGui::TextWrapped("    ");
        AppStyle::pop();

        AppFonts::pushInput();
        AppColors::pushPrimary();
        ImGui::SetNextItemWidth(gamepadLabelWidth);
        ImGui::Text((*gi).owner.guest.name.c_str());
        AppColors::pop();
        AppFonts::pop();

        static ImVec2 backupCursor;
        backupCursor = ImGui::GetCursorPos();

        ImGui::SetCursorPos(cursor);
        ImGui::Button(
            (string("##gamepad button") + to_string(index + 1)).c_str(),
            ImVec2(gamepadLabelWidth, 50.0f)
        );

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("Gamepad", &index, sizeof(int));

            AppFonts::pushInput();
            AppColors::pushPrimary();
            ImGui::Text("%s", ((*gi).owner.guest.isValid() ? (*gi).owner.guest.name.c_str() : "Empty gamepad"));
            AppColors::pop();
            AppFonts::pop();

            AppStyle::pushLabel();
            ImGui::Text("Drop into another Gamepad to swap.");
            AppStyle::pop();

            ImGui::EndDragDropSource();
        }
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
            else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Gamepad"))
            {
                if (payload->DataSize == sizeof(int))
                {
                    int sourceIndex = *(const int*)payload->Data;
                    if (sourceIndex >= 0 && sourceIndex < _gamepads.size())
                    {
                        static GuestDevice backupOwner;
                        backupOwner.copy(_gamepads[index].owner);

                        _gamepads[index].copyOwner(_gamepads[sourceIndex]);
                        _gamepads[sourceIndex].owner.copy(backupOwner);
                    }
                }
            }

            ImGui::EndDragDropTarget();
        }

        ImGui::SetCursorPos(backupCursor);

        ImGui::EndChild();
        
        ImGui::SameLine();
        
        static int padIndices[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        ImGui::BeginGroup();
        ImGui::Dummy(dummySize);
        ImGui::SetNextItemWidth(50);
        padIndices[index] = (*gi).owner.deviceID;

        AppFonts::pushTitle();
        if (ImGui::DragInt(
            (string("##DeviceIndex") + to_string(index)).c_str(),
            &padIndices[index], 0.1f, 0, 65536
        ))
        {
            (*gi).owner.deviceID = padIndices[index];
        }
        AppFonts::pop();
        TitleTooltipWidget::render("Device index", "A guest may have multiple gamepads in the same machine.");
        ImGui::EndGroup();

        ImGui::SameLine();

        //////////////////////////

        ////////////////////////////////////////

        //ImGui::InvisibleButton(
        //    (string("##guest button ") + to_string(index)).c_str(),
        //    ImVec2(size.x, 50.0f)
        //);

        ImGui::EndChild();

        index++;
    }

    //ImGui::Dummy(dummySize);

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