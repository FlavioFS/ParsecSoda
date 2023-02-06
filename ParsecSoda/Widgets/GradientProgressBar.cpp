#include "GradientProgressBar.h"

void GradientProgressBar::renderSingle(const float fraction, const ImVec2& size, const char* overlay, const ImVec4 colorFinal)
{
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
	ImGui::PushStyleColor(ImGuiCol_Border, colorFinal);
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorFinal);
	ImGui::ProgressBar(fraction, size, overlay);
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}

void GradientProgressBar::renderGradient(
	const float fraction, const ImVec2& size, const char* overlay,
	const ImVec4 colorFull, const ImVec4 colorHalf, const ImVec4 colorEmpty
)
{
	static ImVec4 colorFinal;
	colorFinal = lerpColors3(fraction, colorFull, colorHalf, colorEmpty);
	renderSingle(fraction, size, overlay, colorFinal);
}
