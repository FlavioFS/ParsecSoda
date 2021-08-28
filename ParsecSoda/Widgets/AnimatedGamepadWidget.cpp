#include "AnimatedGamepadWidget.h"

void AnimatedGamepadWidget::render(XINPUT_GAMEPAD gamepad, float stickRadius)
{	
	float lDistance = 0, rDistance = 0;
	ImVec2 lStick = stickShortToFloat(gamepad.sThumbLX, gamepad.sThumbLY, lDistance);
	ImVec2 rStick = stickShortToFloat(gamepad.sThumbRX, gamepad.sThumbRY, rDistance);

	static ImU32 primary = ImGui::GetColorU32(IM_COL32(0, 120, 204, 255));
	static ImU32 disabled = ImGui::GetColorU32(IM_COL32(25, 25, 25, 255));
	static float deadzone = 0.1f;

	renderAnalog(lStick, stickRadius, lDistance > deadzone ? primary : disabled);

	ImGui::SameLine();
	ImGui::Dummy(ImVec2(5, 0));
	ImGui::SameLine();

	renderAnalog(rStick, stickRadius, rDistance > deadzone ? primary : disabled);
}

void AnimatedGamepadWidget::renderAnalog(ImVec2 stick, float radius, ImU32 dotColor)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();
	static ImVec2 cursor;
	cursor = ImGui::GetCursorPos();

	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = ImVec2(p0.x + 2*radius, p0.y + 2*radius);
	ImVec2 center = mul(sum(p0, p1), 0.5f);
	ImU32 backgroundColor = ImGui::GetColorU32(IM_COL32(20, 20, 20, 255));
	drawList->AddCircleFilled(center, radius, backgroundColor);

	ImVec2 dotCenter = stick;
	float dotRatio = 0.4f;
	dotCenter.x = center.x + min(max(stick.x, -1.0f), 1.0f) * radius * (1.0f - dotRatio);
	dotCenter.y = center.y - min(max(stick.y, -1.0f), 1.0f) * radius * (1.0f - dotRatio);
	drawList->AddCircleFilled(dotCenter, dotRatio * radius, dotColor);

	ImGui::SetCursorPos(cursor);
	ImGui::Dummy(ImVec2(2*radius, 2*radius));
}

ImVec2 AnimatedGamepadWidget::stickShortToFloat(SHORT lx, SHORT ly, float& distance)
{
	static float shortMax = 32768;
	ImVec2 stick = ImVec2(
		(float)lx / shortMax,
		(float)ly / shortMax
	);

	distance = sqrtf(stick.x * stick.x + stick.y * stick.y);
	if (distance > 0.01f)
	{
		if (abs(lx) > abs(ly))
		{
			if (abs(stick.x) > 0.01f)
			{
				float yy = stick.y / abs(stick.x);
				float xx = 1.0f;
				float maxDiagonal = sqrtf(xx * xx + yy * yy);
				if (maxDiagonal > 0.01f)
				{
					stick.x /= maxDiagonal;
					stick.y /= maxDiagonal;
				}
			}
		}
		else
		{
			if (abs(stick.y) > 0.01f)
			{
				float xx = stick.x / abs(stick.y);
				float yy = 1.0f;
				float maxDiagonal = sqrtf(xx * xx + yy * yy);
				if (maxDiagonal > 0.01f)
				{
					stick.x /= maxDiagonal;
					stick.y /= maxDiagonal;
				}
			}
		}
	}

	return stick;
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