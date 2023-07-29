/**
 * @copyright 2023-present Brian Cairl
 *
 * @file texture_asset_manager.cpp
 */

// C++ Standard Library
#include <memory>
#include <string>
#include <vector>

// Entt
#include <entt/entt.hpp>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

// ImGuiFileDialogue
#include <ImGuiFileDialog.h>

// Tyl
#include <tyl/engine/core/resource.hpp>
#include <tyl/engine/widgets/texture_asset_manager.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/utility/entt.hpp>

namespace tyl::engine::widgets
{
namespace
{

constexpr float kPreviewWidthMin = 50.0;
constexpr float kPreviewWidthMax = 250.0;

struct PreviewState
{
  bool is_selected = false;
  ImVec2 dimensions = {};
};

struct WidgetProperties
{
  bool show_previews = true;
  float preview_width = kPreviewWidthMin;
  ImVec2 preview_icon_dimensions = {};
};

ImVec2 compute_image_dimensions(const graphics::device::Shape2D& shape, const float max_width)
{
  const float ratio = static_cast<float>(shape.width) / static_cast<float>(shape.height);
  const float height = ratio * max_width;
  return ImVec2{max_width, height};
}

float compute_centering_offset_y(const float available_y, const float inner_y) { return 0.5 * (available_y - inner_y); }

}  // namespace

class TextureAssetManager::Impl
{
public:
  Impl() {}

  void update(entt::registry& registry)
  {
    handle_preview_initialization(registry);
    handle_menu(registry);
    handle_file_dialogue(registry);
    handle_error_popup();

    // Handle invidual previews
    if (properties_.show_previews)
    {
      const float x_offset_spacing = std::max(5.f, properties_.preview_width * 0.1f);
      const auto available_space = ImGui::GetContentRegionAvail();
      registry.view<core::resource::Texture::Tag, core::resource::Path, graphics::device::Texture, PreviewState>().each(
        [&, drawlist = ImGui::GetWindowDrawList()](
          const entt::entity id, const auto& path, const auto& texture, auto& state) {
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
            ImGui::SetDragDropPayload("_TEXTURE_ASSET", path.string().c_str(), path.string().size(), /*cond = */ 0);
            ImGui::Text("%s", path.filename().string().c_str());
            ImGui::EndDragDropSource();
          }

          {
            const ImVec2 lower_pos{
              pos.x + x_offset_spacing,
              pos.y + compute_centering_offset_y(properties_.preview_icon_dimensions.y, state.dimensions.y)};
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
              pos.y + compute_centering_offset_y(properties_.preview_icon_dimensions.y, ImGui::GetTextLineHeight())};
            drawlist->AddText(
              lower_pos + ImVec2{state.dimensions.x + x_offset_spacing, 0.f},
              IM_COL32_WHITE,
              path.filename().string().c_str());
          }

          ImGui::Dummy(ImVec2{x_offset_spacing, x_offset_spacing * 0.5f});
          ImGui::Separator();
          ImGui::Dummy(ImVec2{x_offset_spacing, x_offset_spacing * 0.5f});
        });
    }
    else
    {
      registry.view<core::resource::Texture::Tag, core::resource::Path, graphics::device::Texture, PreviewState>().each(
        [&](const entt::entity id, const auto& path, const auto& texture, auto& state) {
          ImGui::Checkbox(path.filename().string().c_str(), &state.is_selected);
          if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
          {
            ImGui::SetDragDropPayload("_TEXTURE_ASSET", path.string().c_str(), path.string().size(), /*cond = */ 0);
            ImGui::Text("%s", path.filename().string().c_str());
            ImGui::EndDragDropSource();
          }
        });
    }
  }

private:
  void recompute_icon_dimensions(entt::registry& registry)
  {
    properties_.preview_icon_dimensions = ImVec2{0, 0};
    registry.template view<core::resource::Texture::Tag, graphics::device::Texture, PreviewState>().each(
      [&](const entt::entity id, const auto& texture, auto& state) {
        state.dimensions = compute_image_dimensions(texture.shape(), properties_.preview_width);
        properties_.preview_icon_dimensions.x = std::max(properties_.preview_icon_dimensions.x, state.dimensions.x);
        properties_.preview_icon_dimensions.y = std::max(properties_.preview_icon_dimensions.y, state.dimensions.y);
      });
  }

  void handle_preview_initialization(entt::registry& registry)
  {
    bool any_initialized = false;

    // Add view state to all available texture resources
    registry.template view<core::resource::Texture::Tag, graphics::device::Texture>(entt::exclude<PreviewState>)
      .each([&](const entt::entity id, const auto& texture) {
        registry.emplace<PreviewState>(id);
        any_initialized = true;
      });

    if (any_initialized)
    {
      recompute_icon_dimensions(registry);
    }
  }

  void handle_menu(entt::registry& registry)
  {
    bool should_recompute_icon_dimensions = false;

    if (ImGui::BeginMenuBar())
    {
      if (ImGui::BeginMenu("file"))
      {
        if (ImGui::MenuItem("import"))
        {
          static constexpr int kInfiniteSelections = 0;
          ImGuiFileDialog::Instance()->OpenDialog("#AssetPicker", "Choose File", ".png,.jpg", ".", kInfiniteSelections);
        }

        if (ImGui::MenuItem("delete"))
        {
          // TODO(qol) show pop-up "are you sure" before deleting
          // TODO(qol) show deleting progress bar
          // TODO(perf) delete in separate thread
          registry.view<core::resource::Texture::Tag, PreviewState>().each(
            [&registry](const entt::entity id, const auto& state) {
              if (state.is_selected)
              {
                core::resource::release(registry, id);
              }
            });
        }

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("select"))
      {
        if (ImGui::MenuItem("all"))
        {
          registry.view<core::resource::Texture::Tag, PreviewState>().each(
            [](const entt::entity id, auto& state) { state.is_selected = true; });
        }

        if (ImGui::MenuItem("none"))
        {
          registry.view<core::resource::Texture::Tag, PreviewState>().each(
            [](const entt::entity id, auto& state) { state.is_selected = false; });
        }

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("view"))
      {
        ImGui::Checkbox("show previews", &properties_.show_previews);
        if (properties_.show_previews)
        {
          should_recompute_icon_dimensions =
            ImGui::SliderFloat("width", &properties_.preview_width, kPreviewWidthMin, kPreviewWidthMax);
        }
        ImGui::EndMenu();
      }

      ImGui::EndMenuBar();
    }

    if (should_recompute_icon_dimensions)
    {
      recompute_icon_dimensions(registry);
    }
  }

  void handle_file_dialogue(entt::registry& registry)
  {
    if (ImGuiFileDialog::Instance()->Display("#AssetPicker"))
    {
      if (ImGuiFileDialog::Instance()->IsOk())
      {
        for (const auto& [filename, file_path_name] : ImGuiFileDialog::Instance()->GetSelection())
        {
          // TODO(perf) do loading in another thread
          // TODO(qol) show loading progress bar
          if (const auto id_or_error =
                core::resource::create(registry, file_path_name, core::resource::TypeCode::TEXTURE);
              !id_or_error.has_value())
          {
            std::ostringstream oss;
            oss << "Error loading [" << file_path_name << "]: " << id_or_error.error();
            last_errors_.emplace_back(oss.str());
          }
        }
      }
      ImGuiFileDialog::Instance()->Close();
    }
  }

  void handle_error_popup()
  {
    if (!last_errors_.empty() && !ImGui::IsPopupOpen("#ErrorDialogue"))
    {
      ImGui::OpenPopup("#ErrorDialogue");
    }

    if (ImGui::BeginPopup("#ErrorDialogue"))
    {
      for (const auto& error : last_errors_)
      {
        ImGui::Text("%s", error.c_str());
      }
      if (ImGui::Button("close"))
      {
        last_errors_.clear();
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  }

  WidgetProperties properties_;
  std::vector<std::string> last_errors_ = {};
};

TextureAssetManager::~TextureAssetManager() = default;

tyl::expected<TextureAssetManager, TextureAssetManager::OnCreateErrorCode>
TextureAssetManager::create(const Options& options)
{
  return TextureAssetManager{options, std::make_unique<Impl>()};
}

TextureAssetManager::TextureAssetManager(const Options& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

void TextureAssetManager::update(ImGuiContext* const imgui_ctx, entt::registry& reg)
{
  ImGui::SetCurrentContext(imgui_ctx);
  if (ImGui::Begin(options_.name, nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar))
  {
    impl_->update(reg);
  }
  ImGui::End();
}

}  // namespace tyl::engine::widgets
