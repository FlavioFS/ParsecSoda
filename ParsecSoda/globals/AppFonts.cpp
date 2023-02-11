#include "AppFonts.h"

ImFont* AppFonts::caption;
ImFont* AppFonts::label;
ImFont* AppFonts::title;
ImFont* AppFonts::input;
ImFont* AppFonts::bigheader;
ImFont* AppFonts::sugoidekai;

void AppFonts::init(ImGuiIO& io)
{
	// Default
	label = io.Fonts->AddFontFromFileTTF(APPFONTS_REGULAR, 14.0f);

	caption = io.Fonts->AddFontFromFileTTF(APPFONTS_BOLD, 12.0f);
	title = io.Fonts->AddFontFromFileTTF(APPFONTS_BOLD, 18.0f);
	input = io.Fonts->AddFontFromFileTTF(APPFONTS_SEMI_BOLD, 16.0f);
	bigheader = io.Fonts->AddFontFromFileTTF(APPFONTS_BOLD, 24.0f);
	sugoidekai = io.Fonts->AddFontFromFileTTF(APPFONTS_BOLD, 36.0f);
}

void AppFonts::pushCaption()
{
	pushFont(caption);
}

void AppFonts::pushLabel()
{
	pushFont(label);
}

void AppFonts::pushTitle()
{
	pushFont(title);
}

void AppFonts::pushInput()
{
	pushFont(input);
}

void AppFonts::pushBigHeader()
{
	pushFont(bigheader);
}

void AppFonts::pushSugoiDekai()
{
	pushFont(sugoidekai);
}

void AppFonts::pushFont(ImFont* font)
{
	ImGui::PushFont(font);
}

void AppFonts::pop()
{
	ImGui::PopFont();
}
