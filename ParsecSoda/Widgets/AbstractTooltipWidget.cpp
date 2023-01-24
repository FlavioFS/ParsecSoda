#include "AbstractTooltipWidget.h"

bool AbstractTooltipWidget::render(const char* title, AbstractTooltipWidget::Action callback, bool forceShow)
{
	if (callback)
	{
		if (ImGui::IsItemHovered() || forceShow)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));

			ImGui::BeginTooltip();
			
			callback();

			ImGui::EndTooltip();

			ImGui::PopStyleVar();
			ImGui::PopStyleVar();

			return true;
		}
	}

	return false;
}
