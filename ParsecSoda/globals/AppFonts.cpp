#include "AppFonts.h"

ImFont* AppFonts::label;
ImFont* AppFonts::title;
ImFont* AppFonts::input;
ImFont* AppFonts::sugoidekai;

void AppFonts::init(ImGuiIO& io)
{
	// Default
	label = io.Fonts->AddFontFromFileTTF(APPFONTS_REGULAR, 16.0f);

	title = io.Fonts->AddFontFromFileTTF(APPFONTS_BOLD, 20.0f);
	input = io.Fonts->AddFontFromFileTTF(APPFONTS_SEMI_BOLD, 16.0f);
	sugoidekai = io.Fonts->AddFontFromFileTTF(APPFONTS_BOLD, 39.0f);
}

void AppFonts::pushLabel()
{
	ImGui::PushFont(label);
}

void AppFonts::pushTitle()
{
	ImGui::PushFont(title);
}

void AppFonts::pushInput()
{
	ImGui::PushFont(input);
}

void AppFonts::pushSugoiDekai()
{
	ImGui::PushFont(sugoidekai);
}

void AppFonts::pop()
{
	ImGui::PopFont();
}
