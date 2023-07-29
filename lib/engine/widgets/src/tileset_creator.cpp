/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tileset_creator.cpp
 */

// C++ Standard Library
#include <memory>
#include <optional>

// Entt
#include <entt/entt.hpp>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/engine/core/resource.hpp>
#include <tyl/engine/widgets/tileset_creator.hpp>
#include <tyl/graphics/device/texture.hpp>

namespace tyl::engine::widgets
{
namespace
{}  // namespace

class TilesetCreator::Impl
{
public:
  Impl() {}

  void update(entt::registry& registry)
  {
    if (!texture_id_.has_value())
    {
      ImGui::Text("%s", "To start, drop a texture here!");
    }
    else if (!registry.valid(*texture_id_))
    {
      texture_id_.reset();
      return;
    }
    else
    {
      const auto& texture = registry.get<graphics::device::Texture>(*texture_id_);
      ImGui::Image(
        reinterpret_cast<void*>(texture.get_id()),
        ImVec2(texture.shape().height, texture.shape().width),
        ImVec2(0, 0),
        ImVec2(1, 1),
        ImVec4(1, 1, 1, 1),
        ImVec4(0, 0, 0, 0));
    }

    if (!ImGui::BeginDragDropTarget())
    {
      return;
    }
    else if (const auto* const payload = ImGui::AcceptDragDropPayload("_TEXTURE_ASSET", /*cond = */ 0);
             payload != nullptr)
    {
      const core::resource::Path asset_path{
        std::string_view{reinterpret_cast<char*>(payload->Data), static_cast<std::size_t>(payload->DataSize)}};
      if (auto guid_or_error = core::resource::get(registry, asset_path); guid_or_error.has_value())
      {
        texture_id_.emplace(std::move(guid_or_error).value());
      }
      else
      {
        // TODO(qol) handle error
      }
    }
    ImGui::EndDragDropTarget();
  }

private:
  std::optional<entt::entity> texture_id_;
};

TilesetCreator::~TilesetCreator() = default;

tyl::expected<TilesetCreator, TilesetCreator::OnCreateErrorCode> TilesetCreator::create(const Options& options)
{
  return TilesetCreator{std::make_unique<Impl>()};
}

TilesetCreator::TilesetCreator(std::unique_ptr<Impl>&& impl) : impl_{std::move(impl)} {}

void TilesetCreator::update(ImGuiContext* const imgui_ctx, entt::registry& reg)
{
  ImGui::SetCurrentContext(imgui_ctx);
  if (ImGui::Begin("TilesetCreator", nullptr, ImGuiWindowFlags_None))
  {
    impl_->update(reg);
  }
  ImGui::End();
}

}  // namespace tyl::engine::widgets
