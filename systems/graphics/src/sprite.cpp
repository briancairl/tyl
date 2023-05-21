/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sprite.cpp
 */

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/graphics/sprite.hpp>
#include <tyl/graphics/sprite_lookup.hpp>


namespace tyl::graphics
{
ecs::entity
create_sprite(ecs::registry& reg, const Vec2f& position, const Size2f& size, const SpriteSheetLookupOptions& options)
{
  const auto e = reg.create();
  attach_sprite(reg, e, position, size, options);
  return e;
}

void attach_sprite(
  ecs::registry& reg,
  const ecs::entity e,
  const Vec2f& position,
  const Size2f& size,
  const SpriteSheetLookupOptions& options)
{
  reg.emplace<SpritePosition>(e, position);
  reg.emplace<SpriteSize>(e, size);
  update_sprite_sheet_bounds(reg.emplace<SpriteSheetLookup>(e), options);
}

void update_sprite(ecs::registry& reg, const ecs::entity e, const SpriteSheetLookupOptions& options)
{
  TYL_ASSERT_TRUE(reg.all_of<SpriteSheetLookup>(e))
  update_sprite_sheet_bounds(reg.get<SpriteSheetLookup>(e), options);
}


void update_sprite(ecs::registry& reg, const ecs::entity e, const Rect2f& uv_bounds)
{
  TYL_ASSERT_TRUE(reg.all_of<SpriteSheetLookup>(e))
  reg.get<SpriteSheetLookup>(e)->emplace_back(uv_bounds);
}

}  // namespace tyl::graphics
