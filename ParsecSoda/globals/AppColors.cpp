#include "AppColors.h"

ImVec4 AppColors::title;
ImVec4 AppColors::label;
ImVec4 AppColors::input;
ImVec4 AppColors::alert;
ImVec4 AppColors::error;

ImVec4 AppColors::white;
ImVec4 AppColors::black;
ImVec4 AppColors::invisible;

void AppColors::init()
{
	title = ImVec4(0.05f, 0.03f, 0.07f, 1.0f);
	label = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	input = ImVec4(0.70f, 0.70f, 0.70f, 1.0f);
	alert = ImVec4(0.00f, 0.67f, 0.41f, 1.00f);
	error = ImVec4(0.75f, 0.16f, 0.28f, 1.00f);

	white = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	black = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	invisible = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
}

ImVec4 AppColors::alpha(ImVec4& color, float alpha)
{
	return ImVec4(color.x, color.y, color.z, alpha);
}

void AppColors::pushTitle()
{
	ImGui::PushStyleColor(ImGuiCol_Text, title);
}

void AppColors::pushLabel()
{
	ImGui::PushStyleColor(ImGuiCol_Text, label);
}

void AppColors::pushInput()
{
	ImGui::PushStyleColor(ImGuiCol_Text, input);
}

void AppColors::pushAlert()
{
	ImGui::PushStyleColor(ImGuiCol_Text, alert);
}

void AppColors::pushError()
{
	ImGui::PushStyleColor(ImGuiCol_Text, error);
}

void AppColors::pop()
{
	ImGui::PopStyleColor();
}