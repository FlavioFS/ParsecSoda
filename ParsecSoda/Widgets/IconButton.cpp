#include "IconButton.h"

bool IconButton::render(Icon icon, ImVec4 tint, ImVec2 size)
{
    static bool isPressed = false;

    isPressed = ImGui::ImageButton(icon, size, ImVec2(0, 0), ImVec2(1, 1), -1, AppColors::invisible, tint);
    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    return isPressed;
}
