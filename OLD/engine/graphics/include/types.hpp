/**
 * @copyright 2023-present Brian Cairl
 *
 * @file primitives_renderer_types.hpp
 */
#pragma once

// C++ Standard Library
#include <vector>

// Tyl
#include <tyl/math/vec.hpp>

namespace tyl::engine::graphics
{

struct DrawType
{
  struct LineList
  {};
  struct LineStrip
  {};
  struct Points
  {};
};

using VertexColor = Vec4f;
using VertexColorList = std::vector<Vec4f>;
using VertexList2D = std::vector<Vec2f>;
using VertexList3D = std::vector<Vec3f>;

}  // namespace tyl::engine::graphics
