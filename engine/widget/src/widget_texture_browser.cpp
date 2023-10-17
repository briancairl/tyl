/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget_texture_browser.cpp
 */

#include <iostream>

// C++ Standard Library
#include <memory>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

// ImGuiFileDialogue
#include <ImGuiFileDialog.h>

// Tyl
#include <tyl/dynamic_bitset.hpp>
#include <tyl/engine/widget_texture_browser.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>
#include <tyl/rect.hpp>

namespace tyl::engine
{
namespace
{
using Image = graphics::host::Image;
using Texture = graphics::device::Texture;

constexpr float kPreviewDimMin = 50.0;
// constexpr float kPreviewDimMax = 250.0;

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

constexpr float ComputeCenteringOffset(const float available_y, const float inner_y)
{
  return 0.5 * (available_y - inner_y);
}

}  // namespace

class TextureBrowser::Impl
{
public:
  Impl() {}

  void Update(Registry& registry, WidgetResources& resources)
  {
    AddTextureBrowserPreviewState(registry);
    if (properties_.show_previews)
    {
      ShowTextureWithPreviews(registry);
    }
    else
    {
      ShowTextureWithPreviews(registry);
    }
  }

  void RecomputeIconDimensions(Registry& registry) const
  {
    registry.template view<Texture, TextureBrowserPreviewState>().each(
      [&](const Texture& texture, TextureBrowserPreviewState& state) {
        state.dimensions = ComputeIconDimensions(texture.shape(), properties_.preview_icon_dimensions);
      });
  }

  void AddTextureBrowserPreviewState(Registry& registry)
  {
    bool any_initialized = false;

    // Add view state to all available texture assets
    registry.template view<Texture>(entt::exclude<TextureBrowserPreviewState>)
      .each([&](const EntityID id, const auto& texture) {
        registry.emplace<TextureBrowserPreviewState>(id);
        any_initialized = true;
      });

    if (any_initialized)
    {
      RecomputeIconDimensions(registry);
    }
  }

  void ShowTextureWithPreviews(Registry& registry)
  {
    const float x_offset_spacing = std::max(5.f, properties_.preview_icon_dimensions.x * 0.1f);
    const auto available_space = ImGui::GetContentRegionAvail();
    registry.view<std::filesystem::path, Texture, TextureBrowserPreviewState>().each(
      [&,
       drawlist = ImGui::GetWindowDrawList()](const EntityID id, const auto& path, const auto& texture, auto& state) {
        const auto pos = ImGui::GetCursorScreenPos();

        {
          drawlist->AddRectFilled(
            pos,
            pos + ImVec2{available_space.x, properties_.preview_icon_dimensions.y},
            state.is_selected ? IM_COL32(100, 100, 25, 255) : IM_COL32(100, 100, 100, 255));
        }

        ImGui::Dummy(ImVec2{available_space.x, properties_.preview_icon_dimensions.y});
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left) and ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
          state.is_selected = !state.is_selected;
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
          if (ImGui::SetDragDropPayload("TYL_TEXTURE_ASSET", std::addressof(id), sizeof(EntityID), /*cond = */ 0))
          {
            std::cerr << "ShowTextureWithPreviews:" << static_cast<int>(id) << std::endl;
            ImGui::Text("%s", path.filename().string().c_str());
          }
          ImGui::EndDragDropSource();
        }

        {
          const ImVec2 lower_pos{
            pos.x + ComputeCenteringOffset(properties_.preview_icon_dimensions.x, state.dimensions.x) +
              x_offset_spacing,
            pos.y + ComputeCenteringOffset(properties_.preview_icon_dimensions.y, state.dimensions.y)};
          drawlist->AddImage(
            reinterpret_cast<void*>(texture.get_id()),
            lower_pos,
            lower_pos + state.dimensions,
            ImVec2(0, 0),
            ImVec2(1, 1));
        }

        {
          const ImVec2 lower_pos{
            pos.x + x_offset_spacing,
            pos.y + ComputeCenteringOffset(properties_.preview_icon_dimensions.y, ImGui::GetTextLineHeight())};
          drawlist->AddText(
            lower_pos + ImVec2{properties_.preview_icon_dimensions.x + x_offset_spacing, 0.f},
            IM_COL32_WHITE,
            path.filename().string().c_str());
        }

        ImGui::Dummy(ImVec2{x_offset_spacing, x_offset_spacing * 0.5f});
        ImGui::Separator();
        ImGui::Dummy(ImVec2{x_offset_spacing, x_offset_spacing * 0.5f});
      });
  }

  TextureBrowserProperties properties_;
};

TextureBrowser::~TextureBrowser() = default;

tyl::expected<TextureBrowser, WidgetCreationError> TextureBrowser::CreateImpl(const TextureBrowserOptions& options)
{
  return TextureBrowser{options, std::make_unique<Impl>()};
}

TextureBrowser::TextureBrowser(const TextureBrowserOptions& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

WidgetStatus TextureBrowser::UpdateImpl(Registry& registry, WidgetResources& resources)
{
  if (ImGui::Begin(options_.name, nullptr, ImGuiWindowFlags_HorizontalScrollbar))
  {
    impl_->Update(registry, resources);
  }
  ImGui::End();
  return WidgetStatus::kOk;
}

}  // namespace tyl::engine