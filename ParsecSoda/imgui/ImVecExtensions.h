#pragma once

#include "imgui.h";

static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs)			{ return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline ImVec2 operator*(const ImVec2& lhs, const float rhs)				{ return ImVec2(lhs.x * rhs, lhs.y * rhs); }
static inline ImVec2 operator/(const ImVec2& lhs, const float rhs)				{ return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)			{ return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)			{ return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec4 operator*(const ImVec4& lhs, const float rhs)				{ return ImVec4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs); }
static inline ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs)			{ return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }

namespace Vector2
{
	static const ImVec2 up =	ImVec2( 0, +1);
	static const ImVec2 down =	ImVec2( 0, -1);
	static const ImVec2 left =	ImVec2(-1,  0);
	static const ImVec2 right =	ImVec2(+1,  0);
	static const ImVec2 one =	ImVec2(+1, +1);
	static const ImVec2 zero =	ImVec2( 0,  0);
	
	static inline const float sqMod(const ImVec2& vec)									{ return vec.x * vec.x + vec.y * vec.y; }
	static inline const ImVec2 lerp(const ImVec2& v1, const ImVec2& v2, const float t)	{ return (v1 * (1.0f - t)) + (v2 * t); }
	static inline const ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a)		{ return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a); }

}

namespace Vector4
{
	static const ImVec4 zero = ImVec4(0, 0, 0, 0);
}