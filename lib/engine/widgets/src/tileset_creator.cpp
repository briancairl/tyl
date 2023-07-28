/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tileset_creator.cpp
 */

// C++ Standard Library
#include <memory>

// Entt
#include <entt/entt.hpp>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

// Tyl
#include <tyl/engine/core/resource.hpp>
#include <tyl/engine/widgets/tileset_creator.hpp>
#include <tyl/graphics/device/texture.hpp>

namespace tyl::engine::widgets
{
namespace
{

struct TextureDisplayProperties
{
  static constexpr float kMinZoom = 0.1f;
  static constexpr float kMaxZoom = 10.f;
  float zoom = kMinZoom;
};

void show_available_textures(entt::registry& registry, const ImVec2& available_space)
{
  // Add view state to all available texture resources
  registry.view<core::resource::Texture::Tag>(entt::exclude<TextureDisplayProperties>)
    .each([&registry](const entt::entity guid) { registry.emplace<TextureDisplayProperties>(guid); });

  // Display available textures
  ImGui::Text("%s", "available textures");
  registry
    .view<core::resource::Texture::Tag, core::resource::Path, graphics::device::Texture, TextureDisplayProperties>()
    .each([&available_space](
            const entt::entity guid, const auto& path, const auto& texture, auto& texture_display_properties) {
      ImGui::PushID(path.string().c_str());
      {
        ImGui::Text("%s", path.string().c_str());

        ImGui::SliderFloat(
          "zoom",
          &texture_display_properties.zoom,
          TextureDisplayProperties::kMinZoom,
          TextureDisplayProperties::kMaxZoom);

        ImGui::Text("guid: %d", static_cast<int>(guid));
        ImGui::Text("size: %d x %d", texture.shape().height, texture.shape().width);

        const float aspect_ratio =
          static_cast<float>(texture.shape().height) / static_cast<float>(texture.shape().width);
        const float display_height = available_space.x * texture_display_properties.zoom;
        const float display_width = aspect_ratio * display_height;

        constexpr bool kShowBorders = true;
        constexpr float kMaxDisplayHeight = 400.f;

        ImGui::BeginChild(
          path.string().c_str(),
          ImVec2{available_space.x, std::min(kMaxDisplayHeight, display_height)},
          kShowBorders,
          ImGuiWindowFlags_HorizontalScrollbar);
        {
          ImGui::Image(reinterpret_cast<void*>(texture.get_id()), ImVec2(display_width, display_height));
        }
        ImGui::EndChild();
      }
      ImGui::PopID();
    });
}

}  // namespace

class TilesetCreator::Impl
{
public:
  Impl() {}

  void update(entt::registry& registry, const ImVec2& available_space)
  {
    show_available_textures(registry, available_space);
  }

private:
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
    impl_->update(reg, ImGui::GetContentRegionAvail());
  }
  ImGui::End();
}

}  // namespace tyl::engine::widgets
