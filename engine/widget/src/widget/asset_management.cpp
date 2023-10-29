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
#include <tyl/engine/scene.hpp>
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
        if (std::filesystem::exists(asset_location.path))
        {
          registry.template emplace<AssetInfo>(
            id,
            resources.now,
            AssetError::kNone,
            std::filesystem::file_size(asset_location.path),
            std::filesystem::status(asset_location.path).type());

          registry.template emplace<AssetLoadingStateType>(
            id, async::post(shared.thread_pool, [path = asset_location.path, load]() { return load(path); }));
        }
        else
        {
          registry.template emplace<AssetInfo>(
            id, resources.now, AssetError::kFailedToLocate, std::uintmax_t{0}, std::filesystem::file_type::none);
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

template <> void AssetManagement::SaveImpl(WidgetOArchive<file_handle_ostream>& oar) {}

template <> void AssetManagement::LoadImpl(WidgetIArchive<file_handle_istream>& iar) {}

WidgetStatus AssetManagement::UpdateImpl(Scene& scene, WidgetSharedState& shared, const WidgetResources& resources)
{
  const auto texture_asset_status = scan<Texture, Image>(
    scene.assets,
    shared,
    resources,
    [](const std::filesystem::path& path) -> expected<Image, AssetError> {
      if (auto image_or_error = Image::load(path); image_or_error.has_value())
      {
        return std::move(image_or_error).value();
      }
      return make_unexpected(AssetError::kFailedToLoad);
    },
    [](Registry& registry, EntityID id, Image&& image) { registry.emplace<Texture>(id, image.texture()); });

  static constexpr auto kStaticWindowFlags = ImGuiWindowFlags_None;
  if (ImGui::Begin(options_.name, nullptr, kStaticWindowFlags))
  {
    if (ImGui::BeginTable("##AssetInfo", 3, ImGuiTableFlags_Resizable))
    {
      ImGui::TableSetupColumn("asset");
      ImGui::TableSetupColumn("loaded");
      ImGui::TableSetupColumn("missing");
      ImGui::TableHeadersRow();

      ImGui::TableNextColumn();
      ImGui::TextUnformatted("textures");
      ImGui::TableNextColumn();
      ImGui::Text("%lu", texture_asset_status.loaded);
      ImGui::TableNextColumn();
      if (texture_asset_status.missed > 0)
      {
        ImGui::TextColored(ImVec4{1, 0, 0, 1}, "%lu", texture_asset_status.missed);
      }
      else
      {
        ImGui::Text("%lu", texture_asset_status.missed);
      }
      ImGui::EndTable();
    }
    if (const std::size_t n = texture_asset_status.loaded + texture_asset_status.missed; n < texture_asset_status.total)
    {
      ImGui::ProgressBar(static_cast<float>(n) / static_cast<float>(texture_asset_status.total));
    }
  }
  ImGui::End();
  return WidgetStatus::kOk;
}

}  // namespace tyl::engine