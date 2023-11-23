/**
 * @copyright 2023-present Brian Cairl
 *
 * @file camera.hpp
 */
#pragma once

// C++ Standard Library
#include <vector>

// Tyl
#include <tyl/engine/math.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/std/vector.hpp>

namespace tyl::engine
{

template <typename T> struct DrawingAttributeList
{
  std::vector<T> values;
};

struct Color
{
  Vec4f rgba;
};

struct ColorList : DrawingAttributeList<Color>
{};
struct LineList2D : DrawingAttributeList<Vec2f>
{};
struct LineList3D : DrawingAttributeList<Vec3f>
{};
struct LineStrip2D : DrawingAttributeList<Vec2f>
{};
struct LineStrip3D : DrawingAttributeList<Vec3f>
{};
struct Points2D : DrawingAttributeList<Vec2f>
{};
struct Points3D : DrawingAttributeList<Vec3f>
{};

struct Rect2D : Rect2f
{
  using Rect2f::Rect2f;
};

}  // namespace tyl::engine

namespace tyl::serialization
{

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, engine::Color> : std::true_type
{};

template <typename ArchiveT, typename T> struct serialize<ArchiveT, engine::DrawingAttributeList<T>>
{
  void operator()(ArchiveT& ar, engine::DrawingAttributeList<T>& attrs) { ar& named{"values", attrs.values}; }
};

template <typename ArchiveT>
struct serialize<ArchiveT, engine::ColorList> : serialize<ArchiveT, engine::DrawingAttributeList<engine::Color>>
{};
template <typename ArchiveT>
struct serialize<ArchiveT, engine::LineList2D> : serialize<ArchiveT, engine::DrawingAttributeList<Vec2f>>
{};
template <typename ArchiveT>
struct serialize<ArchiveT, engine::LineList3D> : serialize<ArchiveT, engine::DrawingAttributeList<Vec3f>>
{};
template <typename ArchiveT>
struct serialize<ArchiveT, engine::LineStrip2D> : serialize<ArchiveT, engine::DrawingAttributeList<Vec2f>>
{};
template <typename ArchiveT>
struct serialize<ArchiveT, engine::LineStrip3D> : serialize<ArchiveT, engine::DrawingAttributeList<Vec3f>>
{};
template <typename ArchiveT>
struct serialize<ArchiveT, engine::Points2D> : serialize<ArchiveT, engine::DrawingAttributeList<Vec2f>>
{};
template <typename ArchiveT>
struct serialize<ArchiveT, engine::Points3D> : serialize<ArchiveT, engine::DrawingAttributeList<Vec3f>>
{};

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, engine::Rect2D> : std::true_type
{};

}  // namespace tyl::serialization
