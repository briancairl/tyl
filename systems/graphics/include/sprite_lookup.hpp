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
 * @brief Lookup from ID to UV offset in an altas texture
 */
using SpriteLookup = alias<std::vector<Rect2f>, decltype("SpriteSheetLookup"_tag)>;

/**
 * @brief Options for dividing an atlas texture into sprite frames
 */
struct SpriteLookupOptions
{
  Vec2i atlas_texture_size_px;
  Vec2i subdivisions;
  Vec2i inner_padding_px;
  Rect2i area_px;
  bool transpose = false;
};

/**
 * @brief Adds sprite data to an existing entity
 */
void attach_sprite_lookup(ecs::registry& reg, const ecs::entity e, const SpriteLookupOptions& options);

}  // namespace tyl::graphics

// Tyl
#include <tyl/serial.hpp>
#include <tyl/serial/math/rect.hpp>
#include <tyl/serial/math/vec.hpp>
#include <tyl/serial/utility/alias.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>::tyl::graphics::SpriteLookupOptions</code> save implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, ::tyl::graphics::SpriteLookupOptions>
{
  void operator()(ArchiveT& ar, ::tyl::graphics::SpriteLookupOptions& options)
  {
    ar& named{"atlas_texture_size_px", options.atlas_texture_size_px};
    ar& named{"subdivisions", options.subdivisions};
    ar& named{"inner_padding_px", options.inner_padding_px};
    ar& named{"area_px", options.area_px};
    ar& named{"transpose", options.transpose};
  }
};

}  // namespace tyl::serialization
