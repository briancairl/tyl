/**
 * @copyright 2022-present Brian Cairl
 *
 * @file renderable.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs/ecs.hpp>

namespace tyl::graphics
{

namespace tags
{
struct rendering_enabled
{};

}  // namespace tags

inline void enable_rendering(ecs::registry& reg, const ecs::entity e) { reg.emplace<tags::rendering_enabled>(e); }

inline void disable_rendering(ecs::registry& reg, const ecs::entity e) { reg.emplace<tags::rendering_enabled>(e); }

}  // namespace tyl::graphics

// Tyl
#include <tyl/serial.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>::tyl::graphics::tags::rendering_enabled</code> serialize implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, ::tyl::graphics::tags::rendering_enabled>
{
  constexpr void operator()(ArchiveT& ar, ::tyl::graphics::tags::rendering_enabled& _tag_) {}
};

}  // namespace tyl::serialization