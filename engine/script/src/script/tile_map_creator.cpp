/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tile_map_creator.cpp
 */

// C++ Standard Library
#include <memory>
#include <optional>
#include <vector>

// Tyl
#include <tyl/assert.hpp>
#include <tyl/engine/asset.hpp>
#include <tyl/engine/ecs.hpp>
#include <tyl/engine/internal/imgui.hpp>
#include <tyl/engine/scene.hpp>
#include <tyl/engine/script/tile_map_creator.hpp>
#include <tyl/engine/tile_map.hpp>
#include <tyl/engine/tile_set.hpp>
#include <tyl/format.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/rect.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/file_stream.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/named_ignored.hpp>
#include <tyl/serialization/packet.hpp>
#include <tyl/serialization/std/optional.hpp>
#include <tyl/serialization/std/string.hpp>
#include <tyl/serialization/std/vector.hpp>

// 1. Create new named tileset
// 2. Select texture
// 3. Select grids

namespace tyl::engine
{
namespace
{
using Texture = graphics::device::Texture;

struct TileMapEditingState
{
  bool is_selected = false;
};

}  // namespace

using namespace tyl::serialization;

class TileMapCreator::Impl
{
public:
  Impl() {}

  void Update(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
  {
    ImGui::InputFloat2("tile size", next_tile_size_.data());
    ImGui::InputInt2("tile dims", next_tile_map_dims_.data());
    if (ImGui::Button("new"))
    {
      const auto id = local_registry_.create();
      local_registry_.emplace<std::string>(id, "tilemap");
      local_registry_.emplace<TileMapEditingState>(id);
      {
        auto& tm = local_registry_.emplace<TileMap>(id);
        tm.tile_size = next_tile_size_;
        tm.sections.resize(next_tile_map_dims_.x(), next_tile_map_dims_.y());
      }
      local_registry_.emplace<Reference<Texture>>(id);
      local_registry_.emplace<Reference<TileSet>>(id);
      editing_tile_map_id_ = id;
    }
    TileMapListing();
  }

  void TileMapListing()
  {
    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_None;
    if (ImGui::BeginChild("#TileMapPreviewsChild", ImVec2{0, 0}, kChildShowBoarders, kChildFlags))
    {
      if (ImGui::BeginTable("##TileMapListing", 6, ImGuiTableFlags_Resizable))
      {
        ImGui::TableSetupColumn("##");
        ImGui::TableSetupColumn("name");
        ImGui::TableSetupColumn("map size");
        ImGui::TableSetupColumn("tile size");
        ImGui::TableSetupColumn("texture");
        ImGui::TableSetupColumn("tile set");
        ImGui::TableHeadersRow();

        std::size_t n_selected = 0;
        local_registry_.view<TileMapEditingState, std::string, TileMap, Reference<Texture>, Reference<TileSet>>().each(
          [&](
            const EntityID id,
            TileMapEditingState& state,
            const std::string& tile_map_name,
            const TileMap& tile_map,
            const Reference<Texture>& atlas_texture_ref,
            const Reference<TileSet>& tile_set_ref) {
            bool is_selected = state.is_selected;

            ImGui::PushID(static_cast<int>(id) + 1);
            ImGui::TableNextColumn();
            {
              ImGui::Checkbox("##", &is_selected);
            }
            ImGui::TableNextColumn();
            {
              ImGui::Text("%s", tile_map_name.c_str());
            }
            ImGui::TableNextColumn();
            {
              ImGui::Text("(%ld x %ld)", tile_map.sections.rows(), tile_map.sections.cols());
            }
            ImGui::TableNextColumn();
            {
              ImGui::Text("(%.2f x %.2f)", tile_map.tile_size.x(), tile_map.tile_size.y());
            }
            ImGui::TableNextColumn();
            {
              if (atlas_texture_ref)
              {
                ImGui::Text("(%d)", static_cast<int>(*atlas_texture_ref.id));
              }
              else
              {
                ImGui::Text("n.a.");
              }
            }
            ImGui::TableNextColumn();
            {
              if (tile_set_ref)
              {
                ImGui::Text("(%d)", static_cast<int>(*tile_set_ref.id));
              }
              else
              {
                ImGui::Text("n.a.");
              }
            }

            if (!atlas_texture_ref or !tile_set_ref)
            {
              ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImColor{1.f, 0.f, 0.f, 0.25f});
            }
            ImGui::PopID();

            state.is_selected = is_selected;

            if (is_selected)
            {
              ++n_selected;
            }
          });
        ImGui::EndTable();
        if (n_selected and ImGui::Button("delete"))
        {
          local_registry_.view<TileMapEditingState>().each([&](const EntityID id, const TileMapEditingState& state) {
            if (state.is_selected)
            {
              local_registry_.destroy(id);
            }
          });
        }
      }
    }
    ImGui::EndChild();
  }

  using TileMapComponents =
    Components<std::string, TileSet, TileMapEditingState, Reference<TileSet>, Reference<Texture>>;

  template <typename OArchive> void Save(OArchive& ar) const
  {
    serializable_registry_t<const TileMapComponents> local_registry{local_registry_};
    (void)local_registry;
    // ar << named{"local_registry", local_registry};
  }

  template <typename IArchive> void Load(IArchive& ar)
  {
    serializable_registry_t<TileMapComponents> local_registry{local_registry_};
    (void)local_registry;
    // ar >> named{"local_registry", local_registry};
  }

private:
  Vec2f next_tile_size_ = {16.F, 16.F};
  Vec2i next_tile_map_dims_ = {10, 10};
  std::optional<EntityID> editing_tile_map_id_;
  Registry local_registry_;
};

using namespace tyl::serialization;

TileMapCreator::~TileMapCreator() = default;

tyl::expected<TileMapCreator, ScriptCreationError> TileMapCreator::CreateImpl(const TileMapCreatorOptions& options)
{
  return TileMapCreator{options, std::make_unique<Impl>()};
}

TileMapCreator::TileMapCreator(const TileMapCreatorOptions& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

template <> void TileMapCreator::SaveImpl(ScriptOArchive<file_handle_ostream>& oar) const { impl_->Save(oar); }

template <> void TileMapCreator::LoadImpl(ScriptIArchive<file_handle_istream>& iar) { impl_->Load(iar); }

ScriptStatus TileMapCreator::UpdateImpl(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
{
  static constexpr auto kStaticWindowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
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

template <typename ArchiveT> struct serialize<ArchiveT, engine::TileMapEditingState>
{
  void operator()(ArchiveT& ar, engine::TileMapEditingState& editing_state) {}
};

}  // tyl::serialization
