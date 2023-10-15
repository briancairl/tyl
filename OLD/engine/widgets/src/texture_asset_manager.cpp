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
#include <tyl/engine/core/asset.hpp>
#include <tyl/engine/core/resources.hpp>
#include <tyl/engine/widgets/texture_asset_manager.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/utility/entt.hpp>

namespace tyl::engine
{
namespace
{

constexpr float kPreviewDimMin = 50.0;
constexpr float kPreviewDimMax = 250.0;


struct PreviewState
{
  bool is_selected = false;
  ImVec2 dimensions = {};
};

struct WidgetProperties
{
  bool show_previews = true;
  ImVec2 preview_icon_dimensions = {kPreviewDimMin, kPreviewDimMin};
};

ImVec2 compute_icon_dimensions(const graphics::device::Shape2D& shape, const ImVec2& max_dimensions)
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

float compute_centering_offset(const float available_y, const float inner_y) { return 0.5 * (available_y - inner_y); }

}  // namespace

class TextureAssetManager::Impl
{
public:
  Impl() {}

  void update(core::Resources& resources)
  {
    auto& registry = resources.registry;

    handle_preview_initialization(registry);
    handle_menu(registry);
    handle_file_dialogue(resources);
    handle_error_popup();

    // Handle invidual previews
    if (properties_.show_previews)
    {
      handle_previews(registry);
    }
    else
    {
      handle_no_previews(registry);
    }
  }

private:
  void recompute_icon_dimensions(entt::registry& registry) const
  {
    registry.template view<core::asset::TextureTag, graphics::device::Texture, PreviewState>().each(
      [&](const entt::entity id, const auto& texture, auto& state) {
        state.dimensions = compute_icon_dimensions(texture.shape(), properties_.preview_icon_dimensions);
      });
  }

  void handle_preview_initialization(entt::registry& registry)
  {
    bool any_initialized = false;

    // Add view state to all available texture assets
    registry.template view<core::asset::TextureTag, graphics::device::Texture>(entt::exclude<PreviewState>)
      .each([&](const entt::entity id, const auto& texture) {
        registry.emplace<PreviewState>(id);
        any_initialized = true;
      });

    if (any_initialized)
    {
      recompute_icon_dimensions(registry);
    }
  }

  void handle_previews(entt::registry& registry)
  {
    const float x_offset_spacing = std::max(5.f, properties_.preview_icon_dimensions.x * 0.1f);
    const auto available_space = ImGui::GetContentRegionAvail();
    registry.view<core::asset::TextureTag, core::asset::Path, graphics::device::Texture, PreviewState>().each(
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
            pos.x + compute_centering_offset(properties_.preview_icon_dimensions.x, state.dimensions.x) +
              x_offset_spacing,
            pos.y + compute_centering_offset(properties_.preview_icon_dimensions.y, state.dimensions.y)};
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
            pos.y + compute_centering_offset(properties_.preview_icon_dimensions.y, ImGui::GetTextLineHeight())};
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

  void handle_no_previews(entt::registry& registry)
  {
    registry.view<core::asset::TextureTag, core::asset::Path, graphics::device::Texture, PreviewState>().each(
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
          registry.view<core::asset::TextureTag, PreviewState>().each(
            [&registry](const entt::entity id, const auto& state) {
              if (state.is_selected)
              {
                core::asset::release(registry, id);
              }
            });
        }

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("select"))
      {
        if (ImGui::MenuItem("all"))
        {
          registry.view<core::asset::TextureTag, PreviewState>().each(
            [](const entt::entity id, auto& state) { state.is_selected = true; });
        }

        if (ImGui::MenuItem("none"))
        {
          registry.view<core::asset::TextureTag, PreviewState>().each(
            [](const entt::entity id, auto& state) { state.is_selected = false; });
        }

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("view"))
      {
        ImGui::Checkbox("show previews", &properties_.show_previews);
        if (properties_.show_previews)
        {
          should_recompute_icon_dimensions = ImGui::SliderFloat2(
            "size", reinterpret_cast<float*>(&properties_.preview_icon_dimensions), kPreviewDimMin, kPreviewDimMax);
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

  void handle_file_dialogue(core::Resources& resources)
  {
    if (ImGuiFileDialog::Instance()->Display("#AssetPicker"))
    {
      if (ImGuiFileDialog::Instance()->IsOk())
      {
        for (const auto& [filename, file_path_name] : ImGuiFileDialog::Instance()->GetSelection())
        {
          // TODO(perf) do loading in another thread
          // TODO(qol) show loading progress bar
          if (const auto id_or_error = core::asset::load(resources, file_path_name, core::asset::TypeCode::kTexture);
              !id_or_error.has_value())
          {
            std::ostringstream oss;
            oss << "Error loading [" << file_path_name << "]: " << id_or_error.error();
            last_errors_.emplace_back(oss.str());
          }
          else
          {
            currently_loading_.emplace_back(std::move(id_or_error).value());
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
  std::vector<entt::entity> currently_loading_ = {};
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

void TextureAssetManager::update(ImGuiContext* const imgui_ctx, core::Resources& resources)
{
  ImGui::SetCurrentContext(imgui_ctx);
  if (ImGui::Begin(options_.name, nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar))
  {
    impl_->update(resources);
  }
  ImGui::End();
}

}  // namespace tyl::engine
