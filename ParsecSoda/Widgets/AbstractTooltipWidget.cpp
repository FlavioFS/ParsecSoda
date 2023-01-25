#include "AbstractTooltipWidget.h"

bool AbstractTooltipWidget::render(const char* title, const char* description, AbstractTooltipWidget::Action callback, bool forceShow)
{
	if (callback)
	{
		if (ImGui::IsItemHovered() || forceShow)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));

			ImGui::BeginTooltip();
			
			// Title
			AppColors::pushPrimary();
			ImGui::Text(title);
			AppColors::pop();

			// Description
			AppStyle::pushLabel();
			ImGui::Text(description);
			AppStyle::pop();
			
			ImGui::Dummy(ImVec2(0, 5));

			// Content
			callback();

			ImGui::EndTooltip();

			ImGui::PopStyleVar();
			ImGui::PopStyleVar();

			return true;
		}
	}

	return false;
}
