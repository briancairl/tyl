/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget_texture_browser.cpp
 */

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/engine/internal/drag_and_drop_images.hpp>
#include <tyl/engine/internal/imgui.hpp>
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

  void Update(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources)
  {
    DragAndDropExternalSink(registry, shared, resources);

    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_None;
    ImGui::BeginChild("#TexturePreviews", ImVec2{0, 0}, kChildShowBoarders, kChildFlags);
    AddTextureBrowserPreviewState(registry);
    if (properties_.show_previews)
    {
      ShowTextureWithPreviews(registry);
    }
    else
    {
      ShowTextureWithPreviews(registry);
    }
    lock_window_movement_ = ImGui::IsWindowHovered();
    ImGui::EndChild();
  }

  void RecomputeIconDimensions(Registry& registry) const
  {
    registry.template view<Texture, TextureBrowserPreviewState>().each(
      [&](const Texture& texture, TextureBrowserPreviewState& state) {
        state.dimensions = ComputeIconDimensions(texture.shape(), properties_.preview_icon_dimensions);
      });
  }

  void AddTextureBrowserPreviewState(Registry& registry) const
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

  void ShowTextureWithPreviews(Registry& registry) const
  {
    const float x_offset_spacing = std::max(5.f, properties_.preview_icon_dimensions.x * 0.1f);
    const auto available_space = ImGui::GetContentRegionAvail();
    registry.view<std::filesystem::path, Texture, TextureBrowserPreviewState>().each(
      [&,
       drawlist = ImGui::GetWindowDrawList()](const EntityID id, const auto& path, const auto& texture, auto& state) {
        ImGui::PushID(static_cast<int>(id));
        DragAndDropInternalSource(registry, id, path, texture, state);
        ImGui::PopID();

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

  static void DragAndDropInternalSource(
    Registry& registry,
    const EntityID id,
    const std::filesystem::path& path,
    const Texture& texture,
    const TextureBrowserPreviewState& state)
  {
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
    {
      if (ImGui::SetDragDropPayload("TYL_TEXTURE_ASSET", std::addressof(id), sizeof(EntityID), /*cond = */ 0))
      {
        ImGui::TextColored(ImVec4{0, 1, 0, 1}, "%s", path.filename().string().c_str());
      }
      else
      {
        ImGui::TextColored(ImVec4{1, 0, 0, 1}, "%s", path.filename().string().c_str());
      }
      ImGui::Image(reinterpret_cast<void*>(texture.get_id()), state.dimensions);
      ImGui::EndDragDropSource();
    }
  }

  void DragAndDropExternalSink(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources)
  {
    auto loaded_or_error = drag_and_drop_images_.update(
      registry, shared, resources, [is_hovered = lock_window_movement_] { return is_hovered; });

    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_None;
    ImGui::BeginChild("#TextureDetails", ImVec2{0, 25}, kChildShowBoarders, kChildFlags);
    if (loaded_or_error.has_value())
    {
      ImGui::Text("%d textures loaded", static_cast<int>(registry.view<Texture>().size()));
    }
    else if (const auto e = loaded_or_error.error(); e.total == 0)
    {
      ImGui::Text("%d textures loaded", static_cast<int>(registry.view<Texture>().size()));
    }
    else
    {
      const float p = static_cast<float>(e.loaded) / static_cast<float>(e.total);
      ImGui::ProgressBar(p);
    }
    ImGui::EndChild();
  }

  constexpr bool LockWindowMovement() const { return lock_window_movement_; }

private:
  bool lock_window_movement_ = false;
  DragAndDropImages drag_and_drop_images_;
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

WidgetStatus TextureBrowser::UpdateImpl(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources)
{
  static constexpr auto kStaticWindowFlags = ImGuiWindowFlags_HorizontalScrollbar;
  if (ImGui::Begin(
        options_.name, nullptr, (impl_->LockWindowMovement() ? ImGuiWindowFlags_NoMove : 0) | kStaticWindowFlags))
  {
    impl_->Update(registry, shared, resources);
  }
  ImGui::End();
  return WidgetStatus::kOk;
}

}  // namespace tyl::engine