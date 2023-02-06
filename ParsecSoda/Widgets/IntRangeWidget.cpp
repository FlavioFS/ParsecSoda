#include "IntRangeWidget.h"

template<typename T>
bool IntRangeWidget::render(const char* id, T& value, T minimum, T maximum, float speed, float width, IntRangeWidget::Action releaseDragCallback)
{
    static bool isDragHover, isArrowClick;
    static string idUp, idDown, idDrag;
    isDragHover = false;
    isArrowClick = false;
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
        if (value < maximum) value++;
        isArrowClick = true;
    }
    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    if (ImGui::ArrowButton(idDown.c_str(), ImGuiDir_Down))
    {
        if (value > minimum) value--;
        isArrowClick = true;
    }
    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    ImGui::PopButtonRepeat();
    ImGui::PopStyleVar();
    ImGui::EndGroup();
    
    ImGui::SameLine();

    AppFonts::pushSugoiDekai();
    ImGui::SetNextItemWidth(width);
    static int temp;
    temp = value;
    ImGui::DragInt(idDrag.c_str(), &temp, 0.05f, minimum, maximum);
    value = temp;
    if (ImGui::IsItemHovered())
    {
        isDragHover = true;
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    if (releaseDragCallback && (isArrowClick || ImGui::IsItemDeactivatedAfterEdit()))
    {
        releaseDragCallback();
    }

    AppFonts::pop();


    AppColors::pop();
    AppStyle::pop();
    ImGui::EndGroup();

    return isDragHover;
}

template bool IntRangeWidget::render<uint32_t>(const char* id, uint32_t& value, uint32_t minimum, uint32_t maximum, float speed, float width, IntRangeWidget::Action releaseDragCallback);
template bool IntRangeWidget::render<uint64_t>(const char* id, uint64_t& value, uint64_t minimum, uint64_t maximum, float speed, float width, IntRangeWidget::Action releaseDragCallback);
template bool IntRangeWidget::render<int32_t>(const char* id, int32_t& value, int32_t minimum, int32_t maximum, float speed, float width, IntRangeWidget::Action releaseDragCallback);
template bool IntRangeWidget::render<int64_t>(const char* id, int64_t& value, int64_t minimum, int64_t maximum, float speed, float width, IntRangeWidget::Action releaseDragCallback);
template bool IntRangeWidget::render<size_t>(const char* id, size_t& value, size_t minimum, size_t maximum, float speed, float width, IntRangeWidget::Action releaseDragCallback);
template bool IntRangeWidget::render<int>(const char* id, int& value, int minimum, int maximum, float speed, float width, IntRangeWidget::Action releaseDragCallback);
template bool IntRangeWidget::render<unsigned int>(const char* id, unsigned int& value, unsigned int minimum, unsigned int maximum, float speed, float width, IntRangeWidget::Action releaseDragCallback);
template bool IntRangeWidget::render<long>(const char* id, long& value, long minimum, long maximum, float speed, float width, IntRangeWidget::Action releaseDragCallback);
template bool IntRangeWidget::render<unsigned long>(const char* id, unsigned long& value, unsigned long minimum, unsigned long maximum, float speed, float width, IntRangeWidget::Action releaseDragCallback);
template bool IntRangeWidget::render<long long>(const char* id, long long& value, long long minimum, long long maximum, float speed, float width, IntRangeWidget::Action releaseDragCallback);
template bool IntRangeWidget::render<unsigned long long>(const char* id, unsigned long long& value, unsigned long long minimum, unsigned long long maximum, float speed, float width, IntRangeWidget::Action releaseDragCallback);