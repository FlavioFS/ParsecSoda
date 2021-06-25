#include "LoadingRingWidget.h"

void LoadingRingWidget::render(bool resetAnimation)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();

	ImVec2 gradient_size = ImVec2(ImGui::CalcItemWidth(), ImGui::GetFrameHeight());
	{
		static float speedSlow = 8.0f, speedFast = 16.0f;
		static float speedMin = speedSlow, speedMax = speedFast;
		static ImU32 color = ImGui::GetColorU32(IM_COL32(0, 119, 204, 255));
		static float angle_min = 0, angle_max = 0;
		static ImVec2 p0, p1, center;

		if (resetAnimation)
		{
			angle_min = 0;
			angle_max = 0;
		}

		p0 = ImGui::GetCursorScreenPos();
		p1 = ImVec2(p0.x + 100, p0.y + 100);
		center = mul(sum(p0, p1), 0.5f);
		color = ImGui::GetColorU32(IM_COL32(0, 119, 204, 255));
		angle_min += (IM_PI / 360.0f) * speedMin;
		angle_max += (IM_PI / 360.0f) * speedMax;
		if (angle_max - angle_min > 2 * IM_PI)
		{
			speedMin = speedFast;
			speedMax = speedSlow;
		}
		else if (angle_min - angle_max > 2 * IM_PI)
		{
			speedMin = speedSlow;
			speedMax = speedFast;
		}
		
		drawList->PathArcTo(center, 25.0f, angle_min, angle_max, 32);
		drawList->PathStroke(color, 0, 10.0f);
	}
}