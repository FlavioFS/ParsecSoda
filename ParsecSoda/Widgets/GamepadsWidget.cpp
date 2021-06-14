#include "GamepadsWidget.h"

GamepadsWidget::GamepadsWidget(Hosting& hosting)
    : _hosting(hosting), _gamepads(hosting.getGamepads())
{
}

bool GamepadsWidget::render()
{
    AppStyle::pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(500, 400), ImVec2(800, 900));
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
        userID = (*gi).getOwner().userID;
        

        ImGui::BeginChild(
            (string("##Gamepad " ) + to_string(index)).c_str(),
            ImVec2(size.x, 50)
        );

        static ImVec2 cursor;
        cursor = ImGui::GetCursorPos();
        
        if (IconButton::render(AppIcons::back, AppColors::alpha(AppColors::white, 0.5f)))
        {
            (*gi).clearOwner();
        }
        if (ImGui::IsItemHovered())
        {
            TooltipWidget::render("Strip gamepad");
        }

        ImGui::SameLine();

        if (ToggleIconButtonWidget::render(AppIcons::padon, AppIcons::padoff, (*gi).isConnected()))
        {
            if ((*gi).isConnected())
                (*gi).disconnect();
            else
                (*gi).connect();
        }
        if (ImGui::IsItemHovered())
        {
            TooltipWidget::render("Disconnect gamepad");
        }

        ImGui::SameLine();
        
        
        ImGui::BeginGroup();
        AppStyle::pushLabel();
        if (userID != (uint32_t)GUEST_ID_ERRORS::NONE)
            ImGui::TextWrapped("(# %d)\t", (*gi).getOwner().userID);
        else
            ImGui::TextWrapped("    ");
        AppStyle::pop();

        ImGui::BeginChild(
            (string("##name ") + to_string(index)).c_str(),
            ImVec2(0.65f * size.x, 20.0f)
        );
        ImGui::TextWrapped((*gi).getOwner().name.c_str());
        ImGui::EndChild();
        ImGui::EndGroup();

        ImGui::SameLine();
        
        static int padIndices[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::PushID(100 * (index + 1));
        ImGui::SetNextItemWidth(50);
        if (ImGui::Combo("", &padIndices[index], " 1 \0 2 \0 3 \0 4 \0 5 \0 6 \0 7 \0 8 \0\0", 8))
        {
            if (padIndices[index] >= 0 && padIndices[index] < 8)
            {
                (*gi).setPadId(padIndices[index]);
            }
        }
        ImGui::PopID();
        if (ImGui::IsItemHovered())
        {
            TooltipWidget::render("An user may have multiple gamepads\nin the same machine.");
        }
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
                        (*gi).setOwner(_hosting.getGuestList()[guestIndex]);
                    }
                    
                }
            }
            ImGui::EndDragDropTarget();
        }

        /*
        static vector<int> guestIndices;
        if (guestIndices.size() < _hosting.getGuestList().size())
        {
            size_t diff = _hosting.getGuestList().size() - guestIndices.size();
            for (size_t j = 0; j < diff; j++)
            {
                guestIndices.push_back(0);
            }
        }
        guestIndices[index] = (*gi).getOwner().index;
        const char ** guestNames = _hosting.getGuestNames();

        static bool isDirty = false;
        isDirty = false;

        if (guestNames != nullptr)
        {
            ImGui::PushID(index+1);
            if (ImGui::Combo("", &guestIndex, guestNames, _hosting.getGuestList().size()))
            {
                isDirty = true;
            }
            ImGui::PopID();
        }
        else
        {
            ImGui::Combo("##guestname", &guestIndex, "\0\0", 1);
        }

        ImGui::SameLine();
        */
        //static bool isDirty = false;
        //isDirty = false;

        

        //if (isDirty)
        //{
        //    if (guestIndex >= 0 && guestIndex < _hosting.getGuestList().size())
        //    {
        //        try
        //        {
        //            Guest newOwner = _hosting.getGuestList()[guestIndex];
        //            _hosting.setOwner((*gi), newOwner, 0);
        //        }
        //        catch (const std::exception&) {}
        //    }

        //    isDirty = false;
        //}

        //ImGui::popup

        

        

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
    if (ImGui::IsItemHovered())
    {
        TooltipWidget::render(_hosting.isGamepadLock() ? "Unlock guest inputs" : "Lock guest inputs");
    }
    ImGui::Unindent(indentDistance);


    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return true;
}