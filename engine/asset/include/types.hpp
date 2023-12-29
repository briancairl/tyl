/**
 * @copyright 2023-present Brian Cairl
 *
 * @file types.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <filesystem>

// Tyl
#include <tyl/engine/asset/types_fwd.hpp>
#include <tyl/engine/common/clock.hpp>
#include <tyl/engine/ecs/types.hpp>

namespace tyl::engine::asset
{

using Label = std::string;

/**
 * @brief Persistent game assets
 */
struct Collection
{
  /// Registry holding persistent game assets
  Registry registry;
};

/**
 * @brief Error code incidating problems with asset loading
 */
enum class Error
{
  kNone,
  kInvalidPath,
  kFailedToLocate,
  kFailedToLoad,
};

/**
 * @brief Code indicating where the asset is stored at a high-level
 */
enum class LocationType
{
  kLocal,
  kRemote,
};

/**
 * @brief Holds the location of an asset
 */
template <typename AssetT> struct Location
{
  /// Path to asset
  std::filesystem::path path;
  /// Type of asset location
  LocationType type = LocationType::kLocal;
};

/**
 * @brief Holds meta information about a loaded asset
 *
 * @warning only added as a component to LOADED assets
 */
struct Info
{
  /// Time at which error occurred
  Clock::Time stamp = Clock::Time::min();
  /// Error type
  Error error = Error::kNone;
  /// Effective size of the asset
  std::uintmax_t size_in_bytes = 0;
  /// File type from which asset was loaded
  std::filesystem::file_type type = std::filesystem::file_type::none;
};

}  // namespace tyl::engine::asset
