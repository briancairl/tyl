/**
 * @copyright 2021-present Brian Cairl
 *
 * @file async.hpp
 */
#pragma once

// C++ Standard Library
#include <filesystem>

// Tyl
#include <tyl/async.hpp>
#include <tyl/clock.hpp>
#include <tyl/expected.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/std/chrono.hpp>
#include <tyl/serialization/std/filesystem.hpp>

namespace tyl::engine
{

/**
 * @brief Error code incidating problems with asset loading
 */
enum class AssetErrorCode
{
  kNone,
  kFailedToLocate,
  kFailedToLoad,
};

/**
 * @brief Holds the location of an asset
 */
template <typename AssetT> struct AssetLocation
{
  /// Path to asset
  std::filesystem::path uri;
};

/**
 * @brief Holds handle to a loading asset or asset error
 */
template <typename AssetT> using AssetLoadingState = async::non_blocking_future<expected<AssetT, AssetErrorCode>>;

/**
 * @brief Used to indicate an asset loading error
 */
struct AssetLoadingError
{
  /// Time at which error occurred
  Clock::Time stamp;
  /// Error type
  AssetErrorCode error;
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
  AssetErrorCode error = AssetErrorCode::kNone;
  /// Effective size of the asset
  std::uintmax_t size_in_bytes = 0;
  /// File type from which asset was loaded
  std::filesystem::file_type type = std::filesystem::file_type::none;
};

}  // namespace tyl::engine

namespace tyl::serialization
{

template <typename ArchiveT, typename AssetT> struct serialize<ArchiveT, engine::AssetLocation<AssetT>>
{
  void operator()(ArchiveT& ar, engine::AssetLocation<AssetT>& asset_location) { ar& named{"uri", asset_location.uri}; }
};

template <typename ArchiveT> struct serialize<ArchiveT, engine::AssetLoadingError>
{
  void operator()(ArchiveT& ar, engine::AssetLoadingError& asset_loading_error)
  {
    ar& named{"stamp", asset_loading_error.stamp};
    ar& named{"error", asset_loading_error.error};
  }
};

}  // namespace tyl::serialization
