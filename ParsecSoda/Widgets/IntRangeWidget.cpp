#include "IntRangeWidget.h"

bool IntRangeWidget::render(const char* id, int& value, int minimum, int maximum, float speed)
{
    static bool isDragHover;
    static string idUp, idDown, idDrag;
    isDragHover = false;
    idUp = (string("##Arrow up") + string(id));
    idDown = (string("##Arrow down") + string(id));
    idDrag = (string("##Drag int") + string(id));

    ImGui::BeginGroup();
    AppStyle::pushInput();
    AppColors::pushPrimary();


    ImGui::BeginGroup();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 0));
    ImGui::PushButtonRepeat(true);
    if (ImGui::ArrowButton(idUp.c_str(), ImGuiDir_Up))
    {
        if (value < 64) value++;
    }
    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    if (ImGui::ArrowButton(idDown.c_str(), ImGuiDir_Down))
    {
        if (value > 0) value--;
    }
    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    ImGui::PopButtonRepeat();
    ImGui::PopStyleVar();
    ImGui::EndGroup();
    
    ImGui::SameLine();

    AppFonts::pushSugoiDekai();
    ImGui::SetNextItemWidth(75.0f);
    ImGui::DragInt(idDrag.c_str(), &value, 0.05f, minimum, maximum);
    if (ImGui::IsItemHovered())
    {
        isDragHover = true;
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    AppFonts::pop();


    AppColors::pop();
    AppStyle::pop();
    ImGui::EndGroup();

    return isDragHover;
}
