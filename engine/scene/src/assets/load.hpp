/**
 * @copyright 2023-present Brian Cairl
 *
 * @file load_assets.hpp
 */
#pragma once

// C++ Standard Library
#include <algorithm>
#include <memory>
#include <numeric>

// Tyl
#include <tyl/async.hpp>
#include <tyl/ecs.hpp>
#include <tyl/engine/assets.hpp>
#include <tyl/engine/shared_state.hpp>
#include <tyl/expected.hpp>

namespace tyl::engine
{

/**
 * @brief Holds handle to a loading asset or asset error
 */
template <typename AssetT> using AssetLoadingState = async::non_blocking_future<expected<AssetT, AssetError>>;

template <typename AssetT, typename IntermediateAssetT = AssetT, typename DoLoadFromPathT, typename DoAddToRegistryT>
AssetLoadingStatus
Load(Registry& registry, SharedState& shared_state, DoLoadFromPathT load_from_path, DoAddToRegistryT add_to_registry)
{
  AssetLoadingStatus status;

  using AssetLocationType = AssetLocation<AssetT>;
  using AssetLoadingStateType = AssetLoadingState<IntermediateAssetT>;

  // Assets which have yet to be loaded
  {
    registry.template view<AssetLocationType>(entt::exclude_t<AssetInfo, AssetLoadingStateType>{})
      .each([&](EntityID id, const auto& asset_location) {
        ++status.total;
        if (std::filesystem::exists(asset_location.path))
        {
          registry.template emplace<AssetInfo>(
            id,
            shared_state.now,
            AssetError::kNone,
            std::filesystem::file_size(asset_location.path),
            std::filesystem::status(asset_location.path).type());

          registry.template emplace<AssetLoadingStateType>(
            id, async::post(shared_state.thread_pool, [path = asset_location.path, load_from_path]() {
              return load_from_path(path);
            }));
        }
        else
        {
          registry.template emplace<AssetInfo>(
            id, shared_state.now, AssetError::kFailedToLocate, std::uintmax_t{0}, std::filesystem::file_type::none);
        }
      });
  }

  // Assets which are currently loading
  {
    registry.template view<AssetLocationType, AssetInfo, AssetLoadingStateType>().each(
      [&](EntityID id, const auto& asset_location, auto& asset_info, auto& asset_loading_state) {
        ++status.total;
        if (!asset_loading_state.valid())
        {
          return;
        }
        else if (auto asset_or_error = std::move(asset_loading_state.get()); asset_or_error.has_value())
        {
          add_to_registry(registry, id, std::move(asset_or_error).value());
        }
        else
        {
          asset_info.error = asset_or_error.error();
        }
        registry.template remove<AssetLoadingStateType>(id);
      });
  }

  // Assets which have already been loaded
  {
    registry.template view<AssetLocationType, AssetInfo>(entt::exclude_t<AssetLoadingStateType>{})
      .each([&](EntityID id, const auto& asset_location, const auto& asset_info) {
        ++status.total;
        if (asset_info.error == AssetError::kNone)
        {
          ++status.loaded;
        }
      });
  }

  return status;
}

}  // namespace tyl::engine