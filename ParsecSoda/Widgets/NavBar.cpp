#include "NavBar.h"

void NavBar::render(
	bool& showHostSettings,
	bool& showGamepads,
	bool& showChat,
	bool& showGuests,
	bool& showLog
)
{
	static ImVec2 iconSize = ImVec2(24, 24);
	static ImVec2 windowSize = ImVec2(24 + 3*8, 24*5 + 6*8);
	static ImVec2 zero = ImVec2(0, 0);
	static ImVec2 padding = ImVec2(8, 8);
	
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, zero);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 0);

	ImGui::Begin(
		"##NavBar", (bool*)0,
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoBringToFrontOnFocus
	);
	ImGui::SetWindowSize(windowSize);
	ImGui::SetWindowPos(zero);
	if (IconButton::render(AppIcons::play, AppColors::primary, iconSize))	showHostSettings = !showHostSettings;
	if (IconButton::render(AppIcons::padOn, AppColors::primary, iconSize))	showGamepads = !showGamepads;
	if (IconButton::render(AppIcons::chat, AppColors::primary, iconSize))	showChat = !showChat;
	if (IconButton::render(AppIcons::users, AppColors::primary, iconSize))	showGuests = !showGuests;
	if (IconButton::render(AppIcons::log, AppColors::primary, iconSize))	showLog = !showLog;
	ImGui::End();

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}