/**
 * @copyright 2023-present Brian Cairl
 *
 * @file loading.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>

// Tyl
#include <tyl/engine/asset/types_fwd.hpp>
#include <tyl/engine/common/resources_fwd.hpp>

namespace tyl::engine::asset
{

/**
 * @brief Loads any unloaded assets
 */
struct LoadStatus
{
  /// Number of assets successfully loaded
  std::size_t loaded = 0;

  /// Number of assets which failed to load
  std::size_t failed = 0;

  /// Total number of assets
  std::size_t total = 0;

  /// Returns the total number of assets still being loaded
  constexpr std::size_t pending() const { return total - (loaded + failed); }

  /// Conversion to indicate loading was, at least, attempted for all assets
  constexpr operator bool() const { return (loaded + failed) == total; }
};

/**
 * @brief Loads any unloaded assets
 *
 * Loads are dispatched to one or more threads
 */
LoadStatus Load(Collection& collection, Resources& resources);

}  // namespace tyl::engine::asset
