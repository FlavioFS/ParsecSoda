#include "GradientProgressBar.h"

void GradientProgressBar::render(
	const float fraction, const ImVec2& size, const char* overlay,
	const ImVec4 colorFull, const ImVec4 colorHalf, const ImVec4 colorEmpty
)
{
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, lerpColors3(colorFull, colorHalf, colorEmpty, fraction));
	ImGui::ProgressBar(fraction, size, overlay);
	ImGui::PopStyleColor();
}
