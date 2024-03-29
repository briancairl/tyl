/**
 * @copyright 2023-present Brian Cairl
 *
 * @file script_texture_browser.cpp
 */

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/engine/asset.hpp>
#include <tyl/engine/internal/imgui.hpp>
#include <tyl/engine/scene.hpp>
#include <tyl/engine/script/texture_browser.hpp>
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
  float preview_icon_extent = kPreviewDimMin;
};

struct TextureBrowserPreviewState
{
  bool is_selected = false;
  ImVec2 dimensions = {};
};

ImVec2 ComputeIconDimensions(const graphics::device::Shape2D& shape, const float width)
{
  const float ratio = static_cast<float>(shape.width) / static_cast<float>(shape.height);
  const float height = ratio * width;
  return ImVec2{width, height};
}

}  // namespace

using namespace tyl::serialization;

class TextureBrowser::Impl
{
public:
  Impl() {}

  void Update(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
  {
    DragAndDropExternalSink(scene, shared, resources);

    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_None;
    AddTextureBrowserPreviewState(scene);
    const std::size_t n_selected = ShowTextureListing(scene);

    if ((n_selected > 0) && ImGui::Button("delete"))
    {
      scene.assets.view<TextureBrowserPreviewState>().each([&](const EntityID id, const auto& state) {
        if (state.is_selected)
        {
          scene.assets.destroy(id);
        }
      });
    }
  }

  void RecomputeIconDimensions(Scene& scene) const
  {
    scene.assets.view<Texture, TextureBrowserPreviewState>().each(
      [&](const Texture& texture, TextureBrowserPreviewState& state) {
        state.dimensions = ComputeIconDimensions(texture.shape(), properties_.preview_icon_extent);
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
  }

  std::size_t ShowTextureListing(Scene& scene) const
  {
    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_None;

    std::size_t n_selected = 0;
    if (ImGui::BeginChild("#TexturePreviewsChild", ImVec2{0, 200}, kChildShowBoarders, kChildFlags))
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
                ImGui::IsItemHovered() and ImGui::BeginTooltip())
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
            if (is_selected)
            {
              ++n_selected;
            }
          });

        ImGui::EndTable();
      }
    }
    ImGui::EndChild();
    return n_selected;
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

    const auto& texture = scene.assets.get<Texture>(id);
    ImGui::Image(reinterpret_cast<void*>(texture.get_id()), state.dimensions, ImVec2{0, 0}, ImVec2{1, 1}, tint);
    ImGui::TextColored(tint, "%s", path.filename().string().c_str());

    ImGui::EndDragDropSource();
    return true;
  }

  void DragAndDropExternalSink(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
  {
    for (const auto& path : resources.drop_payloads)
    {
      const auto id = scene.assets.create();
      scene.assets.emplace<AssetLocation<Texture>>(id, path);
    }
  }

  template <typename OArchive> void Save(OArchive& ar) const { ar << named{"properties", properties_}; }

  template <typename IArchive> void Load(IArchive& ar) { ar >> named{"properties", properties_}; }

private:
  TextureBrowserProperties properties_;
};

TextureBrowser::~TextureBrowser() = default;

template <> void TextureBrowser::SaveImpl(ScriptOArchive<file_handle_ostream>& oar) const { impl_->Save(oar); }

template <> void TextureBrowser::LoadImpl(ScriptIArchive<file_handle_istream>& iar) { impl_->Load(iar); }

tyl::expected<TextureBrowser, ScriptCreationError> TextureBrowser::CreateImpl(const TextureBrowserOptions& options)
{
  return TextureBrowser{options, std::make_unique<Impl>()};
}

TextureBrowser::TextureBrowser(const TextureBrowserOptions& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

ScriptStatus TextureBrowser::UpdateImpl(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
{
  static constexpr auto kStaticWindowFlags = ImGuiWindowFlags_HorizontalScrollbar;
  if (ImGui::Begin(options_.name, nullptr, kStaticWindowFlags))
  {
    impl_->Update(scene, shared, resources);
  }
  ImGui::End();
  return ScriptStatus::kOk;
}

}  // namespace tyl::engine

namespace tyl::serialization
{

template <typename ArchiveT>
struct is_trivially_serializable<ArchiveT, engine::TextureBrowserProperties> : std::true_type
{};

}  // tyl::serialization