#include "AudioControlWidget.h"

bool AudioControlWidget::render(const char* id, int* volume, bool isEnabled, float preview, Icon iconOn, Icon iconOff)
{
	static bool wasClicked = false;
	wasClicked = false;

	static float width;
	width = ImGui::GetContentRegionAvail().x;

	AppStyle::pushInput();
	ImGui::BeginChild(
		(string("###AudioControl") + string(id)).c_str(),
		ImVec2(width, 50.0f)
	);
	
	// Mute button
	if (ToggleIconButtonWidget::render(iconOn, iconOff, isEnabled, AppColors::alert, AppColors::error))
	{
		wasClicked = true;
	}

	ImGui::SameLine();

	// Volume control
	ImGui::BeginGroup();
	AppFonts::pushTitle();
	ImGui::SetNextItemWidth(width - 64.0f);
	ImGui::SliderInt(
		(string("###AudioControlSlider") + string(id)).c_str(),
		volume, 0, 100, "%d%%"
	);
	
	// Preview bar
	if (preview > 0.65f) ImGui::PushStyleColor(ImGuiCol_PlotHistogram, AppColors::error);
	else if (preview > 0.45f) ImGui::PushStyleColor(ImGuiCol_PlotHistogram, AppColors::primary);
	else ImGui::PushStyleColor(ImGuiCol_PlotHistogram, AppColors::alert);
	ImGui::ProgressBar(preview, ImVec2(width - 64.0f, 10.0f), "");
	ImGui::PopStyleColor();

	AppFonts::pop();
	ImGui::EndGroup();

	ImGui::EndChild();
	AppStyle::pop();

	return wasClicked;
}
