#pragma once

#include "imgui.h";

static inline ImVec2 operator*(const ImVec2& lhs, const float rhs)				{ return ImVec2(lhs.x * rhs, lhs.y * rhs); }
static inline ImVec2 operator/(const ImVec2& lhs, const float rhs)				{ return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)			{ return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)			{ return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec4 operator*(const ImVec4& lhs, const float rhs)				{ return ImVec4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs); }
static inline ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs)			{ return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }