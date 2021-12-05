/**
 * @copyright 2021-present Brian Cairl
 *
 * @file components.hpp
 * @brief Contains definitions for all common component types
 */
#pragma once

// Tyl
#include <tyl/strong_alias.hpp>
#include <tyl/vec.hpp>

namespace tyl
{

TYL_DEFINE_STRONG_ALIAS(Position2D, Vec2f);
TYL_DEFINE_STRONG_ALIAS(RectSize2D, Vec2f);
TYL_DEFINE_STRONG_ALIAS(Transform2D, Vec3f);

}  // namespace tyl
