/**
 * @copyright 2023-present Brian Cairl
 *
 * @file asset.hpp
 */
#pragma once

// C++ Standard Library
#include <filesystem>
#include <type_traits>

// Tyl
#include <tyl/clock.hpp>
#include <tyl/ecs.hpp>
#include <tyl/serialization/archive_fwd.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/stream_fwd.hpp>

namespace tyl::engine
{

/**
 * @brief Persistent game assets
 */
struct Assets
{
  /// Registry holding persistent game assets
  Registry registry;
};

/**
 * @brief Error code incidating problems with asset loading
 */
enum class AssetError
{
  kNone,
  kInvalidPath,
  kFailedToLocate,
  kFailedToLoad,
};

/**
 * @brief Code indicating where the asset is stored at a high-level
 */
enum class AssetLocationType
{
  kLocal,
  kRemote,
};

/**
 * @brief Holds the location of an asset
 */
template <typename AssetT> struct AssetLocation
{
  /// Path to asset
  std::filesystem::path path;
  /// Type of asset location
  AssetLocationType type = AssetLocationType::kLocal;
};

/**
 * @brief Holds meta information about a loaded asset
 *
 * @warning only added as a component to LOADED assets
 */
struct AssetInfo
{
  /// Time at which error occurred
  Clock::Time stamp = Clock::Time::min();
  /// Error type
  AssetError error = AssetError::kNone;
  /// Effective size of the asset
  std::uintmax_t size_in_bytes = 0;
  /// File type from which asset was loaded
  std::filesystem::file_type type = std::filesystem::file_type::none;
};

struct SharedState;

struct AssetLoadingStatus
{
  std::size_t loaded = 0;
  std::size_t total = 0;
};

struct AssetLoadingSummary
{
  AssetLoadingStatus textures;
  AssetLoadingStatus sound_data;
};

AssetLoadingSummary LoadAssets(Assets& assets, SharedState& shared);

}  // namespace tyl::engine

namespace tyl::serialization
{

template <> struct save<binary_oarchive<file_handle_ostream>, engine::Assets>
{
  void operator()(binary_oarchive<file_handle_ostream>& ar, const engine::Assets& scene) const;
};

template <> struct load<binary_iarchive<file_handle_istream>, engine::Assets>
{
  void operator()(binary_iarchive<file_handle_istream>& ar, engine::Assets& scene) const;
};

template <> struct save<binary_oarchive<mem_ostream>, engine::Assets>
{
  void operator()(binary_oarchive<mem_ostream>& ar, const engine::Assets& scene) const;
};

template <> struct load<binary_iarchive<mem_istream>, engine::Assets>
{
  void operator()(binary_iarchive<mem_istream>& ar, engine::Assets& scene) const;
};

}  // namespace tyl::serialization
