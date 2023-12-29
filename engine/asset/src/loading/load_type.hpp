/**
 * @copyright 2023-present Brian Cairl
 *
 * @file load_type.hpp
 */
#pragma once

// Tyl
#include <tyl/engine/asset/loading.hpp>
#include <tyl/engine/asset/types.hpp>
#include <tyl/engine/common/resources.hpp>
#include <tyl/engine/ecs/types.hpp>
#include <tyl/expected.hpp>

namespace tyl::engine::asset
{

/**
 * @brief Holds handle to a loading asset or asset error
 */
template <typename AssetT> using LoadingState = async::non_blocking_future<expected<AssetT, Error>>;

template <typename AssetT, typename IntermediateAssetT = AssetT, typename DoLoadFromPathT, typename DoAddToRegistryT>
void LoadType(
  LoadStatus& status,
  Registry& registry,
  Resources& resources,
  DoLoadFromPathT load_from_path,
  DoAddToRegistryT add_to_registry)
{
  using LocationType = Location<AssetT>;
  using LoadingStateType = LoadingState<IntermediateAssetT>;

  // Assets which have yet to be loaded
  {
    registry.template view<LocationType>(entt::exclude_t<Info, LoadingStateType>{})
      .each([&](EntityID id, const auto& asset_location) {
        ++status.total;
        if (std::filesystem::exists(asset_location.path))
        {
          registry.template emplace<Info>(
            id,
            resources.now,
            Error::kNone,
            std::filesystem::file_size(asset_location.path),
            std::filesystem::status(asset_location.path).type());

          registry.template emplace<LoadingStateType>(
            id, async::post(resources.thread_pool, [path = asset_location.path, load_from_path]() {
              return load_from_path(path);
            }));
        }
        else
        {
          registry.template emplace<Info>(
            id, resources.now, Error::kFailedToLocate, std::uintmax_t{0}, std::filesystem::file_type::none);
        }
      });
  }

  // Assets which are currently loading
  {
    registry.template view<LocationType, Info, LoadingStateType>().each(
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
        registry.template remove<LoadingStateType>(id);
      });
  }

  // Assets which have already been loaded
  {
    registry.template view<LocationType, Info>(entt::exclude_t<LoadingStateType>{})
      .each([&](EntityID id, const auto& asset_location, const auto& asset_info) {
        ++status.total;
        if (asset_info.error == Error::kNone)
        {
          ++status.loaded;
        }
      });
  }
}

}  // namespace tyl::engine::asset