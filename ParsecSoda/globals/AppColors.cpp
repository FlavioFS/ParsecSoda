#include "AppColors.h"

ImVec4 AppColors::title;
ImVec4 AppColors::label;
ImVec4 AppColors::input;
ImVec4 AppColors::positive;
ImVec4 AppColors::negative;
ImVec4 AppColors::warning;

ImVec4 AppColors::white;
ImVec4 AppColors::black;
ImVec4 AppColors::invisible;

ImVec4 AppColors::primary;
ImVec4 AppColors::secondary;

ImVec4 AppColors::backgroundIcon;

ImVec4 AppColors::disabled;

void AppColors::init()
{
	title = ImVec4(0.05f, 0.03f, 0.07f, 1.0f);
	label = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	input = ImVec4(0.70f, 0.70f, 0.70f, 1.0f);
	positive = ImVec4(0.00f, 0.67f, 0.41f, 1.00f);
	negative = ImVec4(0.75f, 0.16f, 0.28f, 1.00f);
	warning = ImVec4(0.83f, 0.35f, 0.03f, 1.00f);

	white = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	black = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	invisible = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);

	primary = ImVec4(0.00f, 0.47f, 0.80f, 1.00f);
	secondary = ImVec4(0.02f, 0.29f, 0.53f, 1.00f);

	backgroundIcon = ImVec4(0.08f, 0.07f, 0.07f, 1.00f);

	disabled = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
}

ImVec4 AppColors::alpha(ImVec4& color, float alpha)
{
	return ImVec4(color.x, color.y, color.z, alpha);
}

ImU32 AppColors::toU32(ImVec4& color)
{
	return ImGui::GetColorU32(IM_COL32(255 * color.x, 255 * color.y, 255 * color.z, 255 * color.w));
}

void AppColors::pushColor(ImVec4& color)
{
	ImGui::PushStyleColor(ImGuiCol_Text, color);
}

void AppColors::pop()
{
	ImGui::PopStyleColor();
}

void AppColors::pushPrimary()
{
	pushColor(primary);
}

void AppColors::pushSecondary()
{
	pushColor(secondary);
}

void AppColors::pushTitle()
{
	pushColor(title);
}

void AppColors::pushLabel()
{
	pushColor(label);
}

void AppColors::pushInput()
{
	pushColor(input);
}

void AppColors::pushPositive()
{
	pushColor(positive);
}

void AppColors::pushNegative()
{
	pushColor(negative);
}

void AppColors::pushWarning()
{
	pushColor(warning);
}
