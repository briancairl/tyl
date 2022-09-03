/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sprite.hpp
 */
#pragma once

// C++ Standard Library
#include <vector>

// Tyl
#include <tyl/ecs/ecs.hpp>
#include <tyl/math/rect.hpp>
#include <tyl/math/size.hpp>

namespace tyl::graphics
{

/**
 * @brief Position of sprite
 */
struct SpritePosition
{
  Vec2f position;
  constexpr const auto& v() const { return position; }
};

/**
 * @brief Size of sprite
 */
struct SpriteSize
{
  Vec2f size;
  constexpr const auto& v() const { return size; }
};

/**
 * @brief Options for dividing an atlas texture into sprite frames
 */
struct SpriteSheetLookupOptions
{
  Vec2i atlas_texture_size_px;
  Vec2i subdivisions;
  Vec2i inner_padding_px;
  Rect2i area_px;
  bool transpose = false;
};

/**
 * @brief Lookup from ID to UV offset in an altas texture
 */
struct SpriteSheetLookup
{
  /// Bounds for tiles in texture UV coordinate space
  std::vector<Rect2f> uv_bounds;
  constexpr const auto& v() const { return uv_bounds; }
};

/**
 * @brief Creates a new entity and adds sprite data
 */
ecs::entity
create_sprite(ecs::registry& reg, const Vec2f& position, const Size2f& size, const SpriteSheetLookupOptions& options);

/**
 * @brief Adds sprite data to an existing entity
 */
void attach_sprite(
  ecs::registry& reg,
  const ecs::entity e,
  const Vec2f& position,
  const Size2f& size,
  const SpriteSheetLookupOptions& options);

/**
 * @brief Adds frames to an existing sprite
 */
void update_sprite(ecs::registry& reg, const ecs::entity e, const SpriteSheetLookupOptions& options);

/**
 * @brief Adds single frame to an existing sprite
 */
void update_sprite(ecs::registry& reg, const ecs::entity e, const Rect2f& uv_bounds);

}  // namespace tyl::graphics

// Tyl
#include <tyl/serial.hpp>
#include <tyl/serial/math/rect.hpp>
#include <tyl/serial/math/vec.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>::tyl::graphics::SpriteSheetLookupOptions</code> save implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, ::tyl::graphics::SpriteSheetLookupOptions>
{
  void operator()(ArchiveT& ar, ::tyl::graphics::SpriteSheetLookupOptions& options)
  {
    ar& named{"atlas_texture_size_px", options.atlas_texture_size_px};
    ar& named{"subdivisions", options.subdivisions};
    ar& named{"inner_padding_px", options.inner_padding_px};
    ar& named{"area_px", options.area_px};
    ar& named{"transpose", options.transpose};
  }
};

/**
 * @brief Archive-generic <code>::tyl::graphics::SpritePosition</code> save implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, ::tyl::graphics::SpritePosition>
{
  void operator()(ArchiveT& ar, ::tyl::graphics::SpritePosition& sprite_position)
  {
    ar& named{"position", sprite_position.position};
  }
};

/**
 * @brief Archive-generic <code>::tyl::graphics::SpriteSize</code> save implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, ::tyl::graphics::SpriteSize>
{
  void operator()(ArchiveT& ar, ::tyl::graphics::SpriteSize& sprite_size) { ar& named{"size", sprite_size.size}; }
};

/**
 * @brief Archive-generic <code>::tyl::graphics::SpriteSheetLookup</code> save implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, ::tyl::graphics::SpriteSheetLookup>
{
  void operator()(ArchiveT& ar, ::tyl::graphics::SpriteSheetLookup& lookup)
  {
    ar& named{"uv_bounds", lookup.uv_bounds};
  }
};

}  // namespace tyl::serialization
