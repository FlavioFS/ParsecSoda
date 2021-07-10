#include "HostInfoWidget.h"

HostInfoWidget::HostInfoWidget(Hosting& hosting)
	: _hosting(hosting)
{}

void HostInfoWidget::render()
{
	static ImVec2 windowSize = ImVec2(200, 50);
	static ImVec2 windowPos = ImVec2(0, 0);
	static ImVec2 padding = ImVec2(8, 8);
	static ImVec2 viewportSize;
	viewportSize = ImGui::GetMainViewport()->GetCenter();
	viewportSize.x *= 2;
	viewportSize.y *= 2;
	windowPos.x = viewportSize.x - windowSize.x;
	windowPos.y = viewportSize.y - windowSize.y;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 0);

	ImGui::Begin(
		"##HostInfoWidget", (bool*)0,
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoBringToFrontOnFocus
	);

	ImGui::SetWindowSize(windowSize);
	ImGui::SetWindowPos(windowPos);

	if (_hosting.getHost().isValid())
	{
		AppStyle::pushLabel();
		ImGui::TextWrapped("(# %d)\t", _hosting.getHost().userID);
		AppStyle::pop();
		AppColors::pushPrimary();
		AppFonts::pushInput();
		ImGui::TextWrapped(_hosting.getHost().name.c_str());
		AppFonts::pop();
		AppColors::pop();
	}
	else
	{
		AppStyle::pushLabel();
		ImGui::TextWrapped("(# ???)\t");
		AppStyle::pop();
		AppFonts::pushInput();
		ImGui::TextWrapped("Unknown Host");
		AppFonts::pop();
	}

	ImGui::End();

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}