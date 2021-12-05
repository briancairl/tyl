/**
 * @copyright 2021-present Brian Cairl
 *
 * @file target.hpp
 */
#pragma once

// Tyl
#include <tyl/vec.hpp>

namespace tyl::graphics
{

/**
 * @brief Describes current render target state
 */
struct Target
{
  Vec2i viewport_size;
};

}  // namespace tyl::graphics
