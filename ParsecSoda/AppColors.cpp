#include "AppColors.h"

AppColors::AppColors()
{
	_title = ImVec4(0.05f, 0.03f, 0.07f, 1.0f);
	_label = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	_input = ImVec4(0.70f, 0.70f, 0.70f, 1.0f);
}

void AppColors::pushTitle()
{
	ImGui::PushStyleColor(ImGuiCol_Text, _title);
}

void AppColors::pushLabel()
{
	ImGui::PushStyleColor(ImGuiCol_Text, _label);
}

void AppColors::pushInput()
{
	ImGui::PushStyleColor(ImGuiCol_Text, _input);
}

void AppColors::pop()
{
	ImGui::PopStyleColor();
}
