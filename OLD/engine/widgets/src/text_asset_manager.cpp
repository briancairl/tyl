/**
 * @copyright 2023-present Brian Cairl
 *
 * @file text_asset_manager.cpp
 */

// C++ Standard Library
#include <memory>
#include <sstream>
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
#include <tyl/engine/widgets/text_asset_manager.hpp>
#include <tyl/utility/entt.hpp>

namespace tyl::engine::widgets
{
namespace
{

constexpr float kPreviewHeight = 250.0;
constexpr float kPreviewWidth = 500.0;

struct PreviewProperties
{
  bool is_selected = false;
};

struct WidgetProperties
{
  bool show_previews = false;
};

}  // namespace

class TextAssetManager::Impl
{
public:
  Impl() {}

  void update(core::Resources& resources)
  {
    auto& registry = resources.registry;

    // Add view state to all available texture assets
    registry.template view<core::asset::TextTag>(entt::exclude<PreviewProperties>)
      .each([&registry](const entt::entity guid) { registry.emplace<PreviewProperties>(guid); });

    handle_scrolling();
    handle_menu(registry);
    handle_file_dialogue(resources);
    handle_error_popup();

    // Handle invidual previews
    registry.view<core::asset::TextTag, core::asset::Path, std::string, PreviewProperties>().each(
      [&](const entt::entity guid, const auto& path, const auto& text, auto& properties) {
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

          if (properties_.show_previews)
          {
            ImGui::BeginChild(
              "#TextPreviewPanel_TooltipPreview",
              ImVec2{kPreviewWidth, kPreviewHeight},
              /*show_borders=*/true,
              ImGuiWindowFlags_None);
            ImGui::Text("%s", text.c_str());
            ImGui::EndChild();
          }
        }
        ImGui::EndTooltip();
      });
  }

private:
  void handle_scrolling() {}

  void handle_menu(entt::registry& registry)
  {
    if (ImGui::BeginMenuBar())
    {
      if (ImGui::BeginMenu("file"))
      {
        if (ImGui::MenuItem("import"))
        {
          static constexpr int kInfiniteSelections = 0;
          ImGuiFileDialog::Instance()->OpenDialog(
            "#AssetPicker", "Choose File", ".txt,.glsl,.h*", ".", kInfiniteSelections);
        }

        if (ImGui::MenuItem("delete"))
        {
          // TODO(qol) show pop-up "are you sure" before deleting
          // TODO(qol) show deleting progress bar
          // TODO(perf) delete in separate thread
          registry.view<core::asset::TextTag, PreviewProperties>().each(
            [&registry](const entt::entity guid, const auto& properties) {
              if (properties.is_selected)
              {
                core::asset::release(registry, guid);
              }
            });
        }

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("select"))
      {
        if (ImGui::MenuItem("all"))
        {
          registry.view<core::asset::TextTag, PreviewProperties>().each(
            [](const entt::entity guid, auto& properties) { properties.is_selected = true; });
        }

        if (ImGui::MenuItem("none"))
        {
          registry.view<core::asset::TextTag, PreviewProperties>().each(
            [](const entt::entity guid, auto& properties) { properties.is_selected = false; });
        }

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("view"))
      {
        ImGui::Checkbox("show previews", &properties_.show_previews);
        ImGui::EndMenu();
      }

      ImGui::EndMenuBar();
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
          if (const auto id_or_error = core::asset::load(resources, file_path_name, core::asset::TypeCode::kText);
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

TextAssetManager::~TextAssetManager() = default;

tyl::expected<TextAssetManager, TextAssetManager::OnCreateErrorCode> TextAssetManager::create(const Options& options)
{
  return TextAssetManager{options, std::make_unique<Impl>()};
}

TextAssetManager::TextAssetManager(const Options& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

void TextAssetManager::update(ImGuiContext* const imgui_ctx, core::Resources& resources)
{
  ImGui::SetCurrentContext(imgui_ctx);
  if (ImGui::Begin(options_.name, nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar))
  {
    impl_->update(resources);
  }
  ImGui::End();
}

}  // namespace tyl::engine::widgets
