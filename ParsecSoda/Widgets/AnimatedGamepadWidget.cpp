#include "AnimatedGamepadWidget.h"

void AnimatedGamepadWidget::render(string widgetID, Gamepad& gamepad)
{
	static ImVec2 cursor;
	static ImVec2 size;
	static ImVec2 res = ImVec2(180.0f, 118.0f);

	ImGui::BeginGroup();
	size = ImGui::GetContentRegionAvail();
	cursor = ImGui::GetCursorPos();
	
	drawTrigger();

	//ImGui::Image(AppIcons::gamepad_BG, res);
	ImGui::EndGroup();
}

void AnimatedGamepadWidget::drawTrigger()
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	static ImVec2 cursor, a, b, c, d, uva, uvb, uvc, uvd, triggerCenter, triggerSize;
	//static float angle = IM_PI / 12.0f;
	cursor = ImGui::GetCursorPos();

	//ImGui::Image(AppIcons::gamepad_TriggerBG, ImVec2(32.0f, 46.0f));
	ImGui::Dummy(ImVec2(150, 100));

	ImGui::SetCursorPos(cursor);

	ImVec2 gradient_size = ImVec2(ImGui::CalcItemWidth(), ImGui::GetFrameHeight());
	{
		ImVec2 p0 = ImGui::GetCursorScreenPos();
		ImVec2 p1 = ImVec2(p0.x + 100, p0.y + 100);
		ImVec2 center = mul(sum(p0, p1), 0.5f);
		ImU32 col_a = ImGui::GetColorU32(IM_COL32(54, 54, 54, 255));
		ImU32 col_b = ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));
		static float angle_min_left = IM_PI / 4.0f;
		static float angle_max_left = -IM_PI * 5.0f / 4.0f;
		drawList->PathArcTo(center, 15.0f, angle_min_left, angle_max_left, 20);
		drawList->PathStroke(col_a, 0, 15.0f);
		//ImGui::InvisibleButton("##gradient1", gradient_size);
	}


	//triggerCenter = sum(cursor, ImVec2(100.0f, 20.0f));
	//triggerSize = ImVec2(32.0f, 46.0f);
	//
	//float cos_a = cosf(angle);
	//float sin_a = sinf(angle);

	//float wheel_r_outer = 50.0f;
	//float wheel_r_inner = 45.0f;
	//float wheel_thickness = 2.0f;
	//ImVec2 wheel_center = triggerCenter;

	//ImU32 col_hues[7] = {
	//	ImGui::GetColorU32(IM_COL32(60, 220, 180, 255)),
	//	ImGui::GetColorU32(IM_COL32(60, 220, 185, 255)),
	//	ImGui::GetColorU32(IM_COL32(60, 220, 190, 255)),
	//	ImGui::GetColorU32(IM_COL32(60, 220, 195, 255)),
	//	ImGui::GetColorU32(IM_COL32(60, 220, 200, 255)),
	//	ImGui::GetColorU32(IM_COL32(60, 220, 205, 255)),
	//	ImGui::GetColorU32(IM_COL32(60, 220, 210, 255))
	//};

	// Render Hue Wheel
	//const float aeps = 0.5f / wheel_r_outer; // Half a pixel arc length in radians (2pi cancels out).
	//const int segment_per_arc = ImMax(4, (int)wheel_r_outer / 12);
	//for (int n = 0; n < 6; n++)
	//{
	//	const float a0 = (n) / 6.0f * 2.0f * IM_PI - aeps;
	//	const float a1 = (n + 1.0f) / 6.0f * 2.0f * IM_PI + aeps;
	//	const int vert_start_idx = drawList->VtxBuffer.Size;
	//	drawList->PathArcTo(wheel_center, (wheel_r_inner + wheel_r_outer) * 0.5f, a0, a1, segment_per_arc);
	//	drawList->PathStroke(IM_COL32_WHITE, false, wheel_thickness);
	//	const int vert_end_idx = drawList->VtxBuffer.Size;

	//	// Paint colors over existing vertices
	//	ImVec2 gradient_p0(wheel_center.x + ImCos(a0) * wheel_r_inner, wheel_center.y + ImSin(a0) * wheel_r_inner);
	//	ImVec2 gradient_p1(wheel_center.x + ImCos(a1) * wheel_r_inner, wheel_center.y + ImSin(a1) * wheel_r_inner);
	//	ImGui::ShadeVertsLinearColorGradientKeepAlpha(drawList, vert_start_idx, vert_end_idx, gradient_p0, gradient_p1, col_hues[n], col_hues[n + 1]);
	//}


	//ImVec2 pos[4] =
	//{
	//	sum(triggerCenter, ImRotate(ImVec2(-triggerSize.x * 0.5f, -triggerSize.y * 0.5f), cos_a, sin_a)),
	//	sum(triggerCenter, ImRotate(ImVec2(+triggerSize.x * 0.5f, -triggerSize.y * 0.5f), cos_a, sin_a)),
	//	sum(triggerCenter, ImRotate(ImVec2(+triggerSize.x * 0.5f, +triggerSize.y * 0.5f), cos_a, sin_a)),
	//	sum(triggerCenter, ImRotate(ImVec2(-triggerSize.x * 0.5f, +triggerSize.y * 0.5f), cos_a, sin_a))
	//};
	//ImVec2 uvs[4] =
	//{
	//	ImVec2(0.0f, 0.0f),
	//	ImVec2(1.0f, 0.0f),
	//	ImVec2(1.0f, 1.0f),
	//	ImVec2(0.0f, 1.0f)
	//};

	//drawList->PrimReserve(6, 4);
	//drawList->PushTextureID(AppIcons::gamepad_TriggerBG);
	//drawList->PrimQuadUV(pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], IM_COL32_WHITE);
	//drawList->PopTextureID();
	//drawList->PrimUnreserve(6, 4);

	//RenderImageRotated(AppIcons::gamepad_TriggerBG, triggerCenter, triggerSize, angle);

	//a = ImVec2(cursor);

	//ImGui::Image(AppIcons::gamepad_Trigger, ImVec2(32.0f, 46.0f), ImVec2(0,0), ImVec2(1, 1));
}

void AnimatedGamepadWidget::RenderImageRotated(ImTextureID tex_id, ImVec2 center, ImVec2 size, float angle)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float cos_a = cosf(angle);
	float sin_a = sinf(angle);
	ImVec2 pos[4] =
	{
		sum(center, ImRotate(ImVec2(-size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a)),
		sum(center, ImRotate(ImVec2(+size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a)),
		sum(center, ImRotate(ImVec2(+size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a)),
		sum(center, ImRotate(ImVec2(-size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a))
	};
	ImVec2 uvs[4] =
	{
		ImVec2(0.0f, 0.0f),
		ImVec2(1.0f, 0.0f),
		ImVec2(1.0f, 1.0f),
		ImVec2(0.0f, 1.0f)
	};

	draw_list->AddImageQuad(tex_id, pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], IM_COL32_WHITE);
}