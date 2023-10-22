/**
 * @copyright 2023-present Brian Cairl
 *
 * @file imgui.hpp
 */
#pragma once

// C++ Standard Library
#include <cmath>
#include <cstdlib>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

// Tyl
#include <tyl/vec.hpp>

struct ImTransform
{
  ImVec2 offset = {0, 0};
  float scaling = 1;
};

inline ImVec2 ImTruncate(const ImVec2 pt) { return {std::floor(pt.x), std::floor(pt.y)}; }

inline ImTransform ImInverse(const ImTransform& transform)
{
  const float inv_scaling = 1.f / transform.scaling;
  return ImTransform{.offset = -transform.offset * inv_scaling, .scaling = inv_scaling};
}

inline ImVec2 operator^(const ImTransform& transform, const ImVec2 pt) { return pt * transform.scaling; }

inline ImVec2 operator*(const ImTransform& transform, const ImVec2 pt)
{
  return pt * transform.scaling + transform.offset;
}

inline ImTransform operator*(const ImTransform& lhs, const ImTransform& rhs)
{
  return ImTransform{
    .offset = {lhs.scaling * rhs.offset.x + lhs.offset.x, lhs.scaling * rhs.offset.y + lhs.offset.y},
    .scaling = (lhs.scaling * rhs.scaling)};
}

inline ImVec2 ToImVec2(const tyl::Vec2f& v) { return {v.y(), v.x()}; }

inline tyl::Vec2f FromImVec2(const ImVec2& v) { return {v.y, v.x}; }

inline ImVec4 ImFadeColor(ImVec4 original, float alpha_multiplier)
{
  original.w *= alpha_multiplier;
  return original;
}

inline ImColor ImFadeColor(ImColor original, float alpha_multiplier)
{
  original.Value.w *= alpha_multiplier;
  return original;
}

template <std::size_t N = 64, typename... Ts> const char* ImFmt(const char* fmt, Ts&&... fmt_args)
{
  static char STATIC__text_buffer[N];
  std::snprintf(STATIC__text_buffer, sizeof(STATIC__text_buffer), fmt, std::forward<Ts>(fmt_args)...);
  return STATIC__text_buffer;
}