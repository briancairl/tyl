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
#include <tyl/utility/alias.hpp>

namespace tyl::graphics
{

/**
 * @brief Position of sprite
 */
using SpritePosition = alias<Vec2f, decltype("SpritePosition"_tag)>;

/**
 * @brief Size of sprite
 */
using SpriteSize = alias<Size2f, decltype("SpriteSize"_tag)>;

/**
 * @brief Lookup from ID to UV offset in an altas texture
 */
using SpriteSheetLookup = alias<std::vector<Rect2f>, decltype("SpriteSheetLookup"_tag)>;

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
#include <tyl/serial/utility/alias.hpp>

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

}  // namespace tyl::serialization
