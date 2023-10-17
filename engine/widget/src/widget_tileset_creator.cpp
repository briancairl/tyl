/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget_tileset_creator.cpp
 */

#include <iostream>

// C++ Standard Library
#include <memory>
#include <optional>
#include <vector>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

// ImGuiFileDialogue
#include <ImGuiFileDialog.h>

// Tyl
#include <tyl/assert.hpp>
#include <tyl/dynamic_bitset.hpp>
#include <tyl/engine/widget_tileset_creator.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>
#include <tyl/rect.hpp>

// 1. Create new named tileset
// 2. Select texture
// 3. Select grids

namespace tyl::engine
{

using Image = graphics::host::Image;
using Texture = graphics::device::Texture;

struct TileSetSelection
{
  Vec2i dims = {10, 10};
  Vec2f offset = {0.f, 0.f};
  Vec2f cell_size = {25.f, 25.f};
  dynamic_bitset<std::uint64_t> selected = dynamic_bitset<std::uint64_t>{10 * 10};
};

struct TileSet
{
  Vec2f tile_size;
  std::vector<Rect2f> tiles;
};

class TileSetCreator::Impl
{
public:
  Impl() {}

  void Update(Registry& registry, WidgetResources& resources)
  {
    if (ImGui::BeginTable("#TileSetPanels", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
    {
      // Collumn 0
      {
        ImGui::TableNextColumn();
        ImGui::Text("tile sets");
        TileSetPreview(registry, resources);
        TileSetPopUp(registry, resources);
        TileSetNamingPopUp(registry, resources);
      }

      // Collumn 1
      {
        ImGui::TableNextColumn();
        TileSetAtlasTexture(registry, resources);
      }

      ImGui::EndTable();
    }
  }

  void TileSetPreview(Registry& registry, WidgetResources& resources)
  {
    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_None;
    ImGui::BeginChild("#TileSetPreview", ImVec2{0, 0}, kChildShowBoarders, kChildFlags);
    registry.view<std::string, TileSet>().each([this](EntityID id, const std::string& label, const TileSet& tileset) {
      bool is_active = id == active_tile_set_id_;
      ImGui::PushID(static_cast<int>(id));
      ImGui::Checkbox(label.c_str(), &is_active);
      ImGui::PopID();
      if (is_active)
      {
        active_tile_set_id_ = id;
      }
    });
    ImGui::EndChild();
  }

  void TileSetAtlasTextureDragAndDropInternal(Registry& registry)
  {
    if (!ImGui::BeginDragDropTarget())
    {
      return;
    }
    else if (const auto* texture_payload = ImGui::AcceptDragDropPayload("TYL_TEXTURE_ASSET", /*cond = */ 0);
             texture_payload != nullptr)
    {
      TYL_ASSERT_EQ(texture_payload->DataSize, sizeof(EntityID));
      registry.emplace_or_replace<Reference<Texture>>(
        *active_tile_set_id_, *reinterpret_cast<const EntityID*>(texture_payload->Data));
    }
    ImGui::EndDragDropTarget();
  }

  void TileSetAtlasTextureDragAndDropExternal(Registry& registry, WidgetResources& resources)
  {
    active_tile_set_atlas_texture_hovered_ = ImGui::IsItemHovered();
    if (!active_tile_set_id_)
    {
      return;
    }
    else if (active_tile_set_last_loaded_texture_)
    {
      if (!active_tile_set_last_loaded_texture_->valid())
      {
        return;
      }
      else if (auto loaded = active_tile_set_last_loaded_texture_->get(); loaded.image.has_value())
      {
        // Create texture-asset entity
        const auto id = registry.create();
        registry.emplace<Texture>(id, loaded.image->texture());
        registry.emplace<std::filesystem::path>(id, std::move(loaded.image_path));

        // Create reference to texture
        registry.emplace<Reference<Texture>>(*active_tile_set_id_, id);
      }
      active_tile_set_last_loaded_texture_.reset();
    }
    else if (registry.any_of<Reference<Texture>>(*active_tile_set_id_) || resources.drop_payloads.size() != 1)
    {
      return;
    }
    else if (active_tile_set_atlas_texture_hovered_)
    {
      active_tile_set_last_loaded_texture_.emplace(
        async::post(resources.thread_pool, [image_path = resources.drop_payloads.front()] {
          return LoadedImage{.image = Image::load(image_path), .image_path = std::move(image_path)};
        }));
    }
  }

  void TileSetAtlasTexture(Registry& registry, WidgetResources& resources)
  {
    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_HorizontalScrollbar;
    ImGui::BeginChild("#TileSetAtlasTexture", ImVec2{0, 0}, kChildShowBoarders, kChildFlags);
    if (active_tile_set_id_)
    {
      if (registry.any_of<Reference<Texture>>(*active_tile_set_id_))
      {
        const auto& texture_ref = registry.get<Reference<Texture>>(*active_tile_set_id_);
        const auto& texture = resolve(registry, texture_ref);
        ImGui::Image(
          reinterpret_cast<void*>(texture.get_id()),
          ImVec2(texture.shape().height, texture.shape().width),
          ImVec2(0, 0),
          ImVec2(1, 1),
          ImVec4(1, 1, 1, 1),
          ImVec4(0, 0, 0, 0));
      }
      else
      {
        ImGui::TextColored(ImVec4{1, 0, 0, 1}, "drag tileset texture here");
      }
    }
    ImGui::EndChild();
    TileSetAtlasTextureDragAndDropExternal(registry, resources);
    TileSetAtlasTextureDragAndDropInternal(registry);
  }

  void TileSetPopUp(Registry& registry, WidgetResources& resources)
  {
    static constexpr auto kPopUpName = "#TileSetPopUp";
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
      ImGui::OpenPopup(kPopUpName);
    }

    static constexpr auto kPopUpFlags = ImGuiWindowFlags_HorizontalScrollbar;
    if (!ImGui::BeginPopup(kPopUpName, kPopUpFlags))
    {
      return;
    }

    if (ImGui::BeginMenu("edit"))
    {
      if (ImGui::MenuItem("create"))
      {
        tile_set_naming_pop_up_open_ = true;
        ImGui::CloseCurrentPopup();
      }

      if (ImGui::MenuItem("delete") and active_tile_set_id_)
      {
        registry.destroy(*active_tile_set_id_);
        active_tile_set_id_.reset();
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndMenu();
    }
    ImGui::EndPopup();
  }

  void TileSetNamingPopUp(Registry& registry, WidgetResources& resources)
  {
    static constexpr auto kPopUpName = "#TileSetNamingPopUp";
    if (tile_set_naming_pop_up_open_ and !ImGui::IsPopupOpen(kPopUpName))
    {
      ImGui::OpenPopup(kPopUpName);
    }

    static constexpr auto kPopUpFlags = ImGuiWindowFlags_NoTitleBar;
    if (!ImGui::BeginPopupModal(kPopUpName, &tile_set_naming_pop_up_open_, kPopUpFlags))
    {
      return;
    }

    static constexpr auto kTileSetNameFlags = ImGuiInputTextFlags_EnterReturnsTrue;
    static constexpr const char* kTileSetNameBufferDefault = "new tileset";
    static char kTileSetNameBuffer[100] = "new tileset";
    if (ImGui::InputText("name", kTileSetNameBuffer, sizeof(kTileSetNameBuffer), kTileSetNameFlags))
    {
      {
        const auto id = registry.create();
        registry.emplace<std::string>(id, kTileSetNameBuffer);
        registry.emplace<TileSet>(id);
        registry.emplace<std::vector<TileSetSelection>>(id, std::vector<TileSetSelection>{});
        active_tile_set_id_ = id;
      }
      std::strcpy(kTileSetNameBuffer, kTileSetNameBufferDefault);
      tile_set_naming_pop_up_open_ = false;
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  bool LockWindowMovement() { return active_tile_set_atlas_texture_hovered_; }

  bool tile_set_naming_pop_up_open_ = false;
  bool active_tile_set_atlas_texture_hovered_ = false;
  std::optional<EntityID> active_tile_set_id_;

  struct LoadedImage
  {
    expected<Image, Image::ErrorCode> image;
    std::filesystem::path image_path;
  };
  std::optional<async::non_blocking_future<LoadedImage>> active_tile_set_last_loaded_texture_;
};

TileSetCreator::~TileSetCreator() = default;

tyl::expected<TileSetCreator, WidgetCreationError> TileSetCreator::CreateImpl(const TileSetCreatorOptions& options)
{
  return TileSetCreator{options, std::make_unique<Impl>()};
}

TileSetCreator::TileSetCreator(const TileSetCreatorOptions& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

WidgetStatus TileSetCreator::UpdateImpl(Registry& registry, WidgetResources& resources)
{
  if (ImGui::Begin(
        options_.name,
        nullptr,
        (impl_->LockWindowMovement() ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoScrollbar |
          ImGuiWindowFlags_NoScrollWithMouse))
  {
    impl_->Update(registry, resources);
  }
  ImGui::End();
  return WidgetStatus::kOk;
}

}  // namespace tyl::engine