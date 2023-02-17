#pragma once

#include "imgui/ImVecExtensions.h"

namespace Mouse
{
	static bool isWithinBox(ImVec2 upLeftCorner, ImVec2 downRightCorner)
	{
		static ImVec2 mouse;
		mouse = ImGui::GetMousePos();
		return
			mouse.x >= upLeftCorner.x && mouse.x <= downRightCorner.x &&
			mouse.y >= upLeftCorner.y && mouse.y <= downRightCorner.y
			;
	}

	static bool isWithinCircle(ImVec2 center, float radius)
	{
		return Vector2::sqMod(ImGui::GetMousePos() - center) <= radius * radius;
	}

	static bool checkBoxClick(ImVec2 upLeftCorner, ImVec2 downRightCorner, ImGuiMouseButton mouseBtn)
	{
		return ImGui::IsMouseClicked(mouseBtn) && Mouse::isWithinBox(upLeftCorner, downRightCorner);
	}

	static bool checkCircleClick(ImVec2 center, float radius, ImGuiMouseButton mouseBtn)
	{
		return ImGui::IsMouseClicked(mouseBtn) && Mouse::isWithinCircle(center, radius);
	}
}