#include "ProgressCircularWidget.h"

void ProgressCircularWidget::render(int radius, int thickness, float value, float valueMin, float valueMax, ImU32 activeColor, ImU32 backgroundColor)
{
	static ImDrawList* drawList;
	drawList = ImGui::GetWindowDrawList();
	static ImVec2 cursor;
	cursor = ImGui::GetCursorPos();

	float height = 2 * radius;

	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = ImVec2(p0.x + height, p0.y + height);
	ImVec2 center = Vec2::mul(Vec2::sum(p0, p1), 0.5f);

	value = max(min(value, valueMax), valueMin);
	
	float t = 0;
	float range = abs(valueMax - valueMin);
	if (range > 0)
	{
		t = (float)abs(value - valueMin) / range;
	}

	float ringRadius = radius - thickness * 0.5f;
	float minAngle = -IM_PI * 0.5f;
	float maxAngle = IM_PI * 1.5f;

	drawList->PathArcTo(center, ringRadius, minAngle, maxAngle, 100);
	drawList->PathStroke(backgroundColor, 0, thickness);
	drawList->PathArcTo(center, ringRadius, minAngle, minAngle * (1 - t) + maxAngle * t, 100);
	drawList->PathStroke(activeColor, 0, thickness);

	ImGui::SetCursorPos(cursor);
	ImGui::Dummy(ImVec2(height, height));
}
