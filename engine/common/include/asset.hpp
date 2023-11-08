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
enum class AssetError
{
  kNone,
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
 * @brief Holds handle to a loading asset or asset error
 */
template <typename AssetT> using AssetLoadingState = async::non_blocking_future<expected<AssetT, AssetError>>;

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

}  // namespace tyl::engine

namespace tyl::serialization
{

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, Clock::Time> : std::true_type
{};

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, engine::AssetLocationType> : std::true_type
{};

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, engine::AssetError> : std::true_type
{};

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, engine::AssetInfo> : std::true_type
{};

template <typename ArchiveT, typename AssetT> struct serialize<ArchiveT, engine::AssetLocation<AssetT>>
{
  void operator()(ArchiveT& ar, engine::AssetLocation<AssetT>& asset_location)
  {
    ar& named{"path", asset_location.path};
    ar& named{"type", asset_location.type};
  }
};

}  // namespace tyl::serialization
