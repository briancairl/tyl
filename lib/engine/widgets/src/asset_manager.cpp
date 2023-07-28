/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tileset_creator.cpp
 */

// C++ Standard Library
#include <memory>
#include <sstream>
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
#include <tyl/engine/core/drag_and_drop.hpp>
#include <tyl/engine/core/resource.hpp>
#include <tyl/engine/widgets/asset_manager.hpp>
#include <tyl/graphics/device/texture.hpp>

namespace tyl::engine::widgets
{
namespace
{

struct PreviewProperties
{
  bool is_selected = false;
};

template <typename TagT> void preview_controls(entt::registry& registry)
{
  // Add view state to all available texture resources
  registry.template view<TagT>(entt::exclude<PreviewProperties>).each([&registry](const entt::entity guid) {
    registry.emplace<PreviewProperties>(guid);
  });

  if (ImGui::Button("select all"))
  {
    registry.template view<TagT, PreviewProperties>().each(
      [](const entt::entity guid, auto& properties) { properties.is_selected = true; });
  }
  ImGui::SameLine();
  if (ImGui::Button("select none"))
  {
    registry.template view<TagT, PreviewProperties>().each(
      [](const entt::entity guid, auto& properties) { properties.is_selected = false; });
  }
  ImGui::SameLine();
  if (ImGui::Button("delete"))
  {
    // TODO(qol) show pop-up "are you sure" before deleting
    // TODO(qol) show deleting progress bar
    // TODO(perf) delete in separate thread
    registry.template view<TagT, PreviewProperties>().each(
      [&registry](const entt::entity guid, const auto& properties) {
        if (properties.is_selected)
        {
          registry.destroy(guid);
        }
      });
  }
}

struct TexturePreviewPanelProperties
{
  bool show_previews = false;
  float preview_height = 100.f;
};


void available_texture_preview_panel(entt::registry& registry, TexturePreviewPanelProperties& panel)
{
  static constexpr float kPanelSize = 300;

  if (!ImGui::CollapsingHeader("available textures"))
  {
    return;
  }

  ImGui::BeginChild(
    "#TexturePreviewPanel",
    ImVec2{0, kPanelSize},
    /*show_borders=*/true,
    ImGuiWindowFlags_None);
  {
    ImGui::PushID("#TexturePreviewPanel_TopButtons");
    ImGui::Checkbox("show previews", &panel.show_previews);

    if (panel.show_previews)
    {
      ImGui::SameLine();
      ImGui::SliderFloat("height", &panel.preview_height, 100.0f, 500.0f);
    }

    preview_controls<core::resource::Texture::Tag>(registry);

    ImGui::Separator();

    ImGui::BeginChild(
      "#TexturePreviewPanel_ScrollRegion",
      ImVec2{0, 0},
      /*show_borders=*/true,
      ImGuiWindowFlags_HorizontalScrollbar);
    {
      registry.view<core::resource::Texture::Tag, core::resource::Path, graphics::device::Texture, PreviewProperties>()
        .each([&panel](const entt::entity guid, const auto& path, const auto& texture, auto& properties) {
          ImGui::Checkbox(path.string().c_str(), &properties.is_selected);
          if (!ImGui::IsItemHovered())
          {
            return;
          }
          ImGui::BeginTooltip();
          {
            ImGui::PushID(path.string().c_str());
            ImGui::Text("guid: %d", static_cast<int>(guid));
            ImGui::Text("size: %d x %d", texture.shape().height, texture.shape().width);
            ImGui::PopID();

            if (panel.show_previews)
            {
              const float aspect_ratio =
                static_cast<float>(texture.shape().height) / static_cast<float>(texture.shape().width);
              const float display_height = panel.preview_height;
              const float display_width = aspect_ratio * display_height;

              ImGui::Image(
                reinterpret_cast<void*>(texture.get_id()),
                ImVec2(display_width, display_height),
                ImVec2(0, 0),
                ImVec2(1, 1),
                ImVec4(1, 1, 1, 1),
                ImVec4(0, 0, 0, 0));
            }
          }
          ImGui::EndTooltip();
        });
    }
    ImGui::EndChild();
    ImGui::PopID();
  }
  ImGui::EndChild();
}

struct TextPreviewPanelProperties
{
  bool show_previews = false;
};

void available_text_preview_panel(entt::registry& registry, TextPreviewPanelProperties& panel)
{
  static constexpr float kPanelSize = 300;
  static constexpr float kPreviewHeight = 300;
  static constexpr float kPreviewWidth = 300;

  if (!ImGui::CollapsingHeader("available text"))
  {
    return;
  }

  ImGui::BeginChild(
    "#TextPreviewPanel",
    ImVec2{0, kPanelSize},
    /*show_borders=*/true,
    ImGuiWindowFlags_None);
  {
    ImGui::PushID("#TextPreviewPanel_TopButtons");
    ImGui::Checkbox("show previews", &panel.show_previews);

    preview_controls<core::resource::Text::Tag>(registry);

    ImGui::Separator();

    registry.view<core::resource::Text::Tag, core::resource::Path, std::string, PreviewProperties>().each(
      [&panel](const entt::entity guid, const auto& path, const auto& text, auto& properties) {
        ImGui::Checkbox(path.string().c_str(), &properties.is_selected);
        if (!ImGui::IsItemHovered())
        {
          return;
        }
        ImGui::BeginTooltip();
        {
          ImGui::PushID(path.string().c_str());
          ImGui::Text("guid: %d", static_cast<int>(guid));
          ImGui::Text("size: %d bytes", static_cast<int>(text.size()));
          ImGui::PopID();

          if (panel.show_previews)
          {
            ImGui::BeginChild(
              "#TextPreviewPanel_TooltipPreview",
              ImVec2{kPreviewWidth, kPreviewHeight},
              /*show_borders=*/true,
              ImGuiWindowFlags_None);
            ImGui::TextWrapped("%s", text.c_str());
            ImGui::EndChild();
          }
        }
        ImGui::EndTooltip();
      });
    ImGui::PopID();
  }
  ImGui::EndChild();
}

}  // namespace

class AssetManager::Impl
{
public:
  Impl() {}

  void update(entt::registry& registry)
  {
    // Set drag and drop paths
    auto* const draw_and_drop_data = [&registry]() -> core::DragAndDropData* {
      auto& ctx = registry.ctx();
      if (auto* const data = ctx.find<core::DragAndDropData>(); data == nullptr or data->paths.empty())
      {
        return nullptr;
      }
      else
      {
        return data;
      }
    }();

    if (ImGui::BeginMenuBar())
    {
      if (ImGui::MenuItem("open") and draw_and_drop_data == nullptr)
      {
        static constexpr int kInfiniteSelections = 0;
        ImGuiFileDialog::Instance()->OpenDialog(
          "#AssetPicker", "Choose File", ".png,.jpg,.txt,.glsl", ".", kInfiniteSelections);
      }
      ImGui::EndMenuBar();
    }

    if (draw_and_drop_data != nullptr)
    {
      for (const auto& file_path_name : draw_and_drop_data->paths)
      {
        // TODO(perf) do loading in another thread
        // TODO(qol) show loading progress bar
        if (const auto id_or_error = core::resource::create(registry, file_path_name); !id_or_error.has_value())
        {
          std::ostringstream oss;
          oss << "Error loading [" << file_path_name << "]: " << id_or_error.error();
          last_errors_.emplace_back(oss.str());
        }
      }
    }
    else if (ImGuiFileDialog::Instance()->Display("#AssetPicker"))
    {
      if (ImGuiFileDialog::Instance()->IsOk())
      {
        for (const auto& [filename, file_path_name] : ImGuiFileDialog::Instance()->GetSelection())
        {
          // TODO(perf) do loading in another thread
          // TODO(qol) show loading progress bar
          if (const auto id_or_error = core::resource::create(registry, file_path_name); !id_or_error.has_value())
          {
            std::ostringstream oss;
            oss << "Error loading [" << file_path_name << "]: " << id_or_error.error();
            last_errors_.emplace_back(oss.str());
          }
        }
      }
      ImGuiFileDialog::Instance()->Close();
    }

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

    available_texture_preview_panel(registry, texture_preview_panel_properties_);
    available_text_preview_panel(registry, text_preview_panel_properties_);
  }

private:
  TexturePreviewPanelProperties texture_preview_panel_properties_;
  TextPreviewPanelProperties text_preview_panel_properties_;
  std::vector<std::string> last_errors_ = {};
};

AssetManager::~AssetManager() = default;

tyl::expected<AssetManager, AssetManager::OnCreateErrorCode> AssetManager::create(const Options& options)
{
  return AssetManager{std::make_unique<Impl>()};
}

AssetManager::AssetManager(std::unique_ptr<Impl>&& impl) : impl_{std::move(impl)} {}

void AssetManager::update(ImGuiContext* const imgui_ctx, entt::registry& reg)
{
  ImGui::SetCurrentContext(imgui_ctx);
  if (ImGui::Begin("AssetManager", nullptr, ImGuiWindowFlags_MenuBar))
  {
    impl_->update(reg);
  }
  ImGui::End();
}

}  // namespace tyl::engine::widgets
