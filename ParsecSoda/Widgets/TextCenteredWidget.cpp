#include "TextCenteredWidget.h"

void TextCenteredWidget::render(const char* text)
{
	static float windowWidth, textWidth;

	windowWidth = ImGui::GetWindowWidth();
	textWidth = ImGui::CalcTextSize(text).x;

	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text(text);
}
