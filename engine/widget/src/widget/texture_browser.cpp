/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget_texture_browser.cpp
 */

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/engine/asset.hpp>
#include <tyl/engine/internal/imgui.hpp>
#include <tyl/engine/scene.hpp>
#include <tyl/engine/widget/texture_browser.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/file_stream.hpp>
#include <tyl/serialization/named.hpp>

#include <tyl/rect.hpp>

namespace tyl::engine
{
namespace
{
using Image = graphics::host::Image;
using Texture = graphics::device::Texture;

constexpr float kPreviewDimMin = 50.0;
constexpr float kPreviewDimMax = 500.0;

struct TextureBrowserProperties
{
  bool show_previews = true;
  ImVec2 preview_icon_dimensions = {kPreviewDimMin, kPreviewDimMin};
};

struct TextureBrowserPreviewState
{
  bool is_selected = false;
  ImVec2 dimensions = {};
};

ImVec2 ComputeIconDimensions(const graphics::device::Shape2D& shape, const ImVec2& max_dimensions)
{
  const float ratio = static_cast<float>(shape.width) / static_cast<float>(shape.height);
  const float height = ratio * max_dimensions.x;
  if (height < max_dimensions.y)
  {
    return ImVec2{max_dimensions.x, height};
  }
  else
  {
    const float down_scaling = max_dimensions.y / height;
    return ImVec2{down_scaling * max_dimensions.x, down_scaling * height};
  }
}

}  // namespace

using namespace tyl::serialization;

class TextureBrowser::Impl
{
public:
  Impl() {}

  void Update(Scene& scene, WidgetSharedState& shared, const WidgetResources& resources)
  {
    DragAndDropExternalSink(scene, shared, resources);

    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_None;
    ImGui::BeginChild("#TexturePreviews", ImVec2{0, 0}, kChildShowBoarders, kChildFlags);
    AddTextureBrowserPreviewState(scene);
    ShowTextureListing(scene);
    lock_window_movement_ = ImGui::IsWindowHovered();
    ImGui::EndChild();
  }

  void RecomputeIconDimensions(Scene& scene) const
  {
    scene.assets.view<Texture, TextureBrowserPreviewState>().each(
      [&](const Texture& texture, TextureBrowserPreviewState& state) {
        state.dimensions = ComputeIconDimensions(texture.shape(), properties_.preview_icon_dimensions);
      });
  }

  void AddTextureBrowserPreviewState(Scene& scene)
  {
    bool any_initialized = false;

    // Add view state to all available texture assets
    scene.assets.view<AssetLocation<Texture>, Texture>(entt::exclude<TextureBrowserPreviewState>)
      .each([&](const EntityID id, const auto& texture, const auto& asset_location) {
        scene.assets.emplace<TextureBrowserPreviewState>(id);
        any_initialized = true;
      });

    if (any_initialized)
    {
      RecomputeIconDimensions(scene);
    }

    if (ImGui::Button("delete"))
    {
      scene.assets.view<TextureBrowserPreviewState>().each([&](const EntityID id, const auto& state) {
        if (state.is_selected)
        {
          scene.assets.destroy(id);
        }
      });
    }

    ImGui::SameLine();
    ImGui::Checkbox("show previews", &properties_.show_previews);

    if (properties_.show_previews)
    {
      ImGui::SameLine();
      if (ImGui::SliderFloat2(
            "preview dimensions",
            reinterpret_cast<float*>(&properties_.preview_icon_dimensions),
            kPreviewDimMin,
            kPreviewDimMax))
      {
        RecomputeIconDimensions(scene);
      }
    }
  }

  void ShowTextureListing(Scene& scene) const
  {
    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_None;
    if (ImGui::BeginChild("#TexturePreviewsChild", ImVec2{0, 0}, kChildShowBoarders, kChildFlags))
    {
      if (ImGui::BeginTable("##TextureListing", 4, ImGuiTableFlags_Resizable))
      {
        ImGui::TableSetupColumn("##");
        ImGui::TableSetupColumn("path");
        ImGui::TableSetupColumn("size");
        ImGui::TableSetupColumn("id");
        ImGui::TableHeadersRow();

        scene.assets.view<AssetLocation<Texture>, AssetInfo, TextureBrowserPreviewState>().each(
          [&, drawlist = ImGui::GetWindowDrawList()](
            const EntityID id, const auto& asset_location, const auto& asset_info, auto& state) {
            const bool is_valid = (asset_info.error == AssetError::kNone);
            bool is_selected = state.is_selected;

            ImGui::TableNextColumn();
            {
              ImGui::PushID(static_cast<int>(id) + 1);
              ImGui::Checkbox("##", &is_selected);
              ImGui::PopID();
            }
            ImGui::TableNextColumn();
            {
              ImGui::Text("%s", asset_location.path.filename().string().c_str());
              if (
                is_valid && !DragAndDropInternalSource(scene, id, asset_location.path, state) and
                properties_.show_previews and ImGui::IsItemHovered() and ImGui::BeginTooltip())
              {
                const auto& texture = scene.assets.get<Texture>(id);
                ImGui::Image(reinterpret_cast<void*>(texture.get_id()), state.dimensions);
                ImGui::EndTooltip();
              }
            }
            ImGui::TableNextColumn();
            {
              if (is_valid)
              {
                ImGui::Text("%lu kb", asset_info.size_in_bytes / 1000);
              }
              else
              {
                ImGui::Text("[n/a]");
              }
            }
            ImGui::TableNextColumn();
            {
              ImGui::Text("%d", static_cast<int>(id));
            }

            if (!is_valid)
            {
              ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImColor{1.f, 0.f, 0.f, 0.25f});
            }
            else if (is_selected)
            {
              ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImColor{1.f, 1.f, 0.f, 0.25f});
            }
            state.is_selected = is_selected;
          });

        ImGui::EndTable();
      }
      ImGui::EndChild();
    }
  }

  bool DragAndDropInternalSource(
    const Scene& scene,
    const EntityID id,
    const std::filesystem::path& path,
    const TextureBrowserPreviewState& state) const
  {
    if (!ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
    {
      return false;
    }

    ImVec4 tint{1, 1, 1, 1};
    if (ImGui::SetDragDropPayload("TYL_TEXTURE_ASSET", std::addressof(id), sizeof(EntityID), /*cond = */ 0))
    {
      tint = ImVec4{0, 1, 0, 1};
    }

    if (properties_.show_previews)
    {
      const auto& texture = scene.assets.get<Texture>(id);
      ImGui::Image(reinterpret_cast<void*>(texture.get_id()), state.dimensions, ImVec2{0, 0}, ImVec2{1, 1}, tint);
    }
    ImGui::TextColored(tint, "%s", path.filename().string().c_str());

    ImGui::EndDragDropSource();
    return true;
  }

  void DragAndDropExternalSink(Scene& scene, WidgetSharedState& shared, const WidgetResources& resources)
  {
    for (const auto& path : resources.drop_payloads)
    {
      const auto id = scene.assets.create();
      scene.assets.emplace<AssetLocation<Texture>>(id, path);
    }
  }

  constexpr bool LockWindowMovement() const { return lock_window_movement_; }

  template <typename OArchive> void Save(OArchive& ar) const { ar << named{"properties", properties_}; }

  template <typename IArchive> void Load(IArchive& ar) { ar >> named{"properties", properties_}; }

private:
  bool lock_window_movement_ = false;
  TextureBrowserProperties properties_;
};

TextureBrowser::~TextureBrowser() = default;

template <> void TextureBrowser::SaveImpl(WidgetOArchive<file_handle_ostream>& oar) const { impl_->Save(oar); }

template <> void TextureBrowser::LoadImpl(WidgetIArchive<file_handle_istream>& iar) { impl_->Load(iar); }

tyl::expected<TextureBrowser, WidgetCreationError> TextureBrowser::CreateImpl(const TextureBrowserOptions& options)
{
  return TextureBrowser{options, std::make_unique<Impl>()};
}

TextureBrowser::TextureBrowser(const TextureBrowserOptions& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

WidgetStatus TextureBrowser::UpdateImpl(Scene& scene, WidgetSharedState& shared, const WidgetResources& resources)
{
  static constexpr auto kStaticWindowFlags = ImGuiWindowFlags_HorizontalScrollbar;
  if (ImGui::Begin(
        options_.name, nullptr, (impl_->LockWindowMovement() ? ImGuiWindowFlags_NoMove : 0) | kStaticWindowFlags))
  {
    impl_->Update(scene, shared, resources);
  }
  ImGui::End();
  return WidgetStatus::kOk;
}

}  // namespace tyl::engine

namespace tyl::serialization
{

template <typename ArchiveT>
struct is_trivially_serializable<ArchiveT, engine::TextureBrowserProperties> : std::true_type
{};

}  // tyl::serialization