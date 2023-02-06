#include "NavBar.h"

void NavBar::render(
	Hosting& hosting,
	bool& showLogin,
	bool& showHostSettings,
	bool& showGamepads,
	bool& showMasterOfPuppets,
	bool& showHotseats,
	bool& showChat,
	bool& showGuests,
	bool& showThumbnails,
	bool& showLog,
	bool& showAudio,
	bool& showVideo,
	bool& showInfo
)
{
	static const size_t ICON_COUNT = 12;
	static const float ICON_EDGE = 24;
	static const float BUTTON_EDGE = ICON_EDGE + 8;
	static const float ICON_PADDING = 4;

	static ImVec2 iconSize = ImVec2(ICON_EDGE, ICON_EDGE);
	static ImVec2 padding = ImVec2(ICON_PADDING, ICON_PADDING);
	
	static ImVec2 windowSize = ImVec2(ICON_PADDING * 2 + BUTTON_EDGE, ICON_COUNT * (BUTTON_EDGE + ICON_PADDING) - 2 * ICON_PADDING);
	static ImVec2 zero = ImVec2(0, 0);
	
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

	
	renderNavItem(AppIcons::play, AppIcons::play, showHostSettings, iconSize, "Host settings");
	renderNavItem(AppIcons::padOn, AppIcons::padOn, showGamepads, iconSize, "Gamepads");
	renderNavItem(AppIcons::seat, AppIcons::seat, showHotseats, iconSize, "Hotseats");
	renderNavItem(AppIcons::puppet, AppIcons::puppet, showMasterOfPuppets, iconSize, "Master of Puppets");
	renderNavItem(AppIcons::chat, AppIcons::chat, showChat, iconSize, "Chat");
	renderNavItem(AppIcons::users, AppIcons::users, showGuests, iconSize, "Guests");
	renderNavItem(AppIcons::image, AppIcons::image, showThumbnails, iconSize, "Public Thumbnails (Arcade)");
	renderNavItem(AppIcons::log, AppIcons::log, showLog, iconSize, "Log");
	renderNavItem(AppIcons::speakersOn, AppIcons::speakersOn, showAudio, iconSize, "Audio");
	renderNavItem(AppIcons::video, AppIcons::video, showVideo, iconSize, "Video");
	renderNavItem(AppIcons::info, AppIcons::info, showInfo, iconSize, "About");


	if (IconButton::render(AppIcons::logoff, AppColors::primary, iconSize))
	{
		if (!hosting.isRunning())
		{
			showLogin = !showLogin;
		}
	}
	TitleTooltipWidget::render("Log off", "Go back to log in screen.\n\n * Stop streaming before trying to log out.");

	ImGui::End();

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}

void NavBar::renderNavItem(Icon onIcon, Icon offIcon, bool& toggle, ImVec2 size, const char* tooltip)
{
	if (ToggleIconButtonWidget::render(onIcon, offIcon, toggle, size)) toggle = !toggle;
	renderNavTooltip(tooltip, toggle);
}

void NavBar::renderNavTooltip(const char* title, bool& show)
{
	TitleTooltipWidget::render(title, show ? "Window is visible." : "Window is hidden.");
}
