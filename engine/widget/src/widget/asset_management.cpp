/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget_texture_browser.cpp
 */

// C++ Standard Library
#include <algorithm>
#include <memory>
#include <numeric>

// Tyl
#include <tyl/async.hpp>
#include <tyl/engine/asset.hpp>
#include <tyl/engine/ecs.hpp>
#include <tyl/engine/internal/imgui.hpp>
#include <tyl/engine/widget/asset_management.hpp>
#include <tyl/serialization/file_stream.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/packet.hpp>
#include <tyl/serialization/std/vector.hpp>

#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>

namespace tyl::serialization
{}  // tyl::serialization

namespace tyl::engine
{
using Image = graphics::host::Image;
using Texture = graphics::device::Texture;
namespace
{

struct ScanStatus
{
  std::size_t missed = 0;
  std::size_t loaded = 0;
  std::size_t total = 0;
};

template <typename AssetT, typename IntermediateAssetT = AssetT, typename LoadT, typename AddToRegistryT>
ScanStatus scan(
  Registry& registry,
  WidgetSharedState& shared,
  const WidgetResources& resources,
  LoadT load,
  AddToRegistryT add_to_registry)
{
  ScanStatus status;

  using AssetLocationType = AssetLocation<AssetT>;
  using AssetLoadingStateType = AssetLoadingState<IntermediateAssetT>;

  // Assets which have yet to be loaded
  {
    registry.template view<AssetLocationType>(entt::exclude_t<AssetInfo, AssetLoadingStateType>{})
      .each([&](EntityID id, const auto& asset_location) {
        ++status.total;
        if (std::filesystem::exists(asset_location.uri))
        {
          registry.template emplace<AssetInfo>(
            id,
            resources.now,
            AssetErrorCode::kNone,
            std::filesystem::file_size(asset_location.uri),
            std::filesystem::status(asset_location.uri).type());

          registry.template emplace<AssetLoadingStateType>(
            id, async::post(shared.thread_pool, [uri = asset_location.uri, load]() { return load(uri); }));
        }
        else
        {
          registry.template emplace<AssetInfo>(
            id, resources.now, AssetErrorCode::kFailedToLocate, std::uintmax_t{0}, std::filesystem::file_type::none);
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
        if (asset_info.error == AssetErrorCode::kNone)
        {
          ++status.loaded;
        }
        else
        {
          ++status.missed;
        }
      });
  }

  return status;
}

}  // namespace

using namespace tyl::serialization;

AssetManagement::~AssetManagement() = default;

tyl::expected<AssetManagement, WidgetCreationError> AssetManagement::CreateImpl(const AssetManagementOptions& options)
{
  return AssetManagement{options};
}

AssetManagement::AssetManagement(const AssetManagementOptions& options) : options_{options} {}

template <> void AssetManagement::SaveImpl(WidgetOArchive<file_handle_ostream>& oar, const Registry& registry)
{
  const ConstRegistryComponents<AssetLocation<Texture>> textures{registry};
  oar << named{"textures", textures};
}

template <> void AssetManagement::LoadImpl(WidgetIArchive<file_handle_istream>& iar, Registry& registry)
{
  RegistryComponents<AssetLocation<Texture>> textures{registry};
  iar >> named{"textures", textures};
}

WidgetStatus
AssetManagement::UpdateImpl(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources)
{
  const auto texture_asset_status = scan<Texture, Image>(
    registry,
    shared,
    resources,
    [](const std::filesystem::path& path) -> expected<Image, AssetErrorCode> {
      if (auto image_or_error = Image::load(path); image_or_error.has_value())
      {
        return std::move(image_or_error).value();
      }
      else
      {
        return make_unexpected(AssetErrorCode::kFailedToLoad);
      }
    },
    [](Registry& registry, EntityID id, Image&& image) { registry.template emplace<Texture>(id, image.texture()); });

  static constexpr auto kStaticWindowFlags = ImGuiWindowFlags_None;
  if (ImGui::Begin(options_.load_bar_popup_title, nullptr, kStaticWindowFlags))
  {
    ImGui::Text("textures loaded: [%lu]", texture_asset_status.loaded);
    ImGui::Text("textures missed: [%lu]", texture_asset_status.missed);
    if (const std::size_t n = texture_asset_status.loaded + texture_asset_status.missed; n < texture_asset_status.total)
    {
      ImGui::ProgressBar(static_cast<float>(n) / static_cast<float>(texture_asset_status.total));
    }
  }
  ImGui::End();
  return WidgetStatus::kOk;
}

}  // namespace tyl::engine