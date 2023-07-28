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

constexpr float kPreviewHeightMin = 100.0;
constexpr float kPreviewHeightMax = 500.0;
constexpr float kPreviewHeightStep = 10.0;

struct PreviewProperties
{
  bool is_selected = false;
};

struct WidgetProperties
{
  bool show_previews = false;
  float preview_height = kPreviewHeightMin;
};

}  // namespace

class TextureAssetManager::Impl
{
public:
  Impl() {}

  void update(entt::registry& registry)
  {
    // Add view state to all available texture resources
    registry.template view<core::resource::Texture::Tag>(entt::exclude<PreviewProperties>)
      .each([&registry](const entt::entity guid) { registry.emplace<PreviewProperties>(guid); });

    handle_scrolling();
    handle_menu(registry);
    handle_file_dialogue(registry);
    handle_error_popup();

    // Handle invidual previews
    registry.view<core::resource::Texture::Tag, core::resource::Path, graphics::device::Texture, PreviewProperties>()
      .each([&](const entt::entity guid, const auto& path, const auto& texture, auto& properties) {
        ImGui::Checkbox(path.filename().string().c_str(), &properties.is_selected);
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

          if (properties_.show_previews)
          {
            const float aspect_ratio =
              static_cast<float>(texture.shape().height) / static_cast<float>(texture.shape().width);
            const float display_height = properties_.preview_height;
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

private:
  void handle_scrolling()
  {
    if (ImGui::IsWindowHovered())
    {
      if (const auto scroll = ImGui::GetIO().MouseWheel; scroll > 0.f)
      {
        properties_.preview_height = std::min(properties_.preview_height + kPreviewHeightStep, kPreviewHeightMax);
      }
      else if (scroll < 0.f)
      {
        properties_.preview_height = std::max(properties_.preview_height - kPreviewHeightStep, kPreviewHeightMin);
      }
    }
  }

  void handle_menu(entt::registry& registry)
  {
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
          registry.view<core::resource::Texture::Tag, PreviewProperties>().each(
            [&registry](const entt::entity guid, const auto& properties) {
              if (properties.is_selected)
              {
                core::resource::release(registry, guid);
              }
            });
        }

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("select"))
      {
        if (ImGui::MenuItem("all"))
        {
          registry.view<core::resource::Texture::Tag, PreviewProperties>().each(
            [](const entt::entity guid, auto& properties) { properties.is_selected = true; });
        }

        if (ImGui::MenuItem("none"))
        {
          registry.view<core::resource::Texture::Tag, PreviewProperties>().each(
            [](const entt::entity guid, auto& properties) { properties.is_selected = false; });
        }

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("view"))
      {
        ImGui::Checkbox("show previews", &properties_.show_previews);
        ImGui::SliderFloat("height", &properties_.preview_height, kPreviewHeightMin, kPreviewHeightMax);
        ImGui::EndMenu();
      }

      ImGui::EndMenuBar();
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
