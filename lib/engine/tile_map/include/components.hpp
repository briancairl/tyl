/**
 * @copyright 2021-present Brian Cairl
 *
 * @file components.hpp
 */
#pragma once

// Tyl
#include <tyl/matrix.hpp>
#include <tyl/strong_alias.hpp>

namespace tyl::tile_map
{

/// Matrix of tile IDs for an (N x M) tile map
TYL_DEFINE_STRONG_ALIAS(IDMat, MatXi);

/// Tile size in pixels
TYL_DEFINE_STRONG_ALIAS(TileSizePx, Vec2i);

}  // namespace tyl::tile_map
