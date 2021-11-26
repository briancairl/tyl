/**
 * @copyright 2021-present Brian Cairl
 *
 * @file state.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/matrix.hpp>
#include <tyl/strong_alias.hpp>

namespace tyl::graphics
{

/// Size of uniform block (e.g tile in a spritesheet)
TYL_DEFINE_STRONG_ALIAS(TileSizePx, Vec2i);

/// Total size (e.g dimensions of a spritesheet)
TYL_DEFINE_STRONG_ALIAS(TotalSizePx, Vec2i);

namespace tags
{

using namespace entt::literals;

using updated = entt::tag<"tyl.graphics.updated"_hs>;

}  // namespace tags
}  // namespace tyl::graphics
