#include "HostGamepadWidget.h"

bool HostGamepadWidget::render()
{
    static ImVec2 size;
    size = ImGui::GetWindowSize();
    
    //static int hostPadID = 0;
    
    //static XINPUT_STATE hostPad;
    //XInputGetState((DWORD)hostPadID, &hostPad);

    AppStyle::pushTitle();

    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 100), ImVec2(400, 400));
    ImGui::Begin("Host Gamepad##Host Gamepad");
    AppStyle::pushInput();
    
    //IntRangeWidget::render("Host Pad ID", hostPadID, 0, 8);

    static XINPUT_STATE hostPads[8];
    for (DWORD i = 0; i < 8; ++i)
    {
        XInputGetState(i, &hostPads[i]);

        ImGui::Text("[%d]: ", i);
        ImGui::SameLine();

        ImGui::Text((hostPads[i].Gamepad.wButtons & XUSB_GAMEPAD_A) != 0 ? "A" : "?");
        ImGui::SameLine();

        ImGui::Text((hostPads[i].Gamepad.wButtons & XUSB_GAMEPAD_B) != 0 ? "B" : "?");
        ImGui::SameLine();

        ImGui::Text((hostPads[i].Gamepad.wButtons & XUSB_GAMEPAD_X) != 0 ? "X" : "?");
        ImGui::SameLine();

        ImGui::Text((hostPads[i].Gamepad.wButtons & XUSB_GAMEPAD_Y) != 0 ? "Y" : "?");
    }

    AppStyle::pop();
    AppStyle::pop();
    ImGui::End();

    return true;
}
