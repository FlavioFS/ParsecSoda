#pragma once

#include "imgui.h"
#include "../globals/AppColors.h"

namespace ImGui
{
	void loadStyle()
	{
		ImGuiStyle *style = &ImGui::GetStyle();

		// Layout
		style->WindowBorderSize = 1;
		style->PopupBorderSize = 1;
		style->WindowPadding = ImVec2(20, 20);
		style->ItemSpacing = ImVec2(5, 5);
		style->ScrollbarSize = 20;
		style->GrabMinSize = 20;
		
		style->WindowRounding = 0;
		style->FrameRounding = 4;
		style->PopupRounding = 4;
		style->ScrollbarRounding = 4;
		style->GrabRounding = 2;
		style->WindowTitleAlign = ImVec2(0.5f, 0.5f);

		style->DisplaySafeAreaPadding = ImVec2(20, 20);

		// Colors
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.90f);
		colors[ImGuiCol_Border] = ImVec4(0.18f, 0.17f, 0.16f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.08f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.08f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.08f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.39f, 0.47f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = AppColors::primary;
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.32f, 0.39f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.08f, 0.08f, 0.07f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.08f, 0.08f, 0.07f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = AppColors::secondary;
		colors[ImGuiCol_ScrollbarGrabHovered] = AppColors::primary;
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.12f, 0.59f, 0.92f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
		colors[ImGuiCol_SliderGrab] = AppColors::primary;
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 0.39f, 0.47f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.08f);
		colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
		colors[ImGuiCol_Header] = AppColors::secondary;
		colors[ImGuiCol_HeaderHovered] = AppColors::primary;
		colors[ImGuiCol_HeaderActive] = ImVec4(0.12f, 0.59f, 0.92f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
		colors[ImGuiCol_Tab] = AppColors::secondary;
		colors[ImGuiCol_TabHovered] = AppColors::primary;
		colors[ImGuiCol_TabActive] = ImVec4(0.12f, 0.59f, 0.92f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.39f, 0.47f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.05f, 0.49f, 0.58f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = AppColors::primary;
		colors[ImGuiCol_PlotHistogram] = AppColors::primary;
		colors[ImGuiCol_PlotHistogramHovered] = AppColors::secondary;
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.27f, 0.27f, 0.38f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.45f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
		colors[ImGuiCol_TextSelectedBg] = AppColors::secondary;
		colors[ImGuiCol_DragDropTarget] = AppColors::primary;
		colors[ImGuiCol_NavHighlight] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.11f, 0.18f, 0.35f);
	}
}