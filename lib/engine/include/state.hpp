/**
 * @copyright 2021-present Brian Cairl
 *
 * @file state.hpp
 */
#pragma once

namespace tyl::engine
{

struct UpdateFlags
{
  std::size_t has_render_changes : 1;
};

}  // namespace tyl::engine
