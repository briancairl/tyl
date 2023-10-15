/**
 * @copyright 2023-present Brian Cairl
 *
 * @file drag_and_drop.cpp
 */

// C++ Standard Library

// Entt
#include <entt/entt.hpp>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

// Tyl
#include <tyl/engine/core/asset.hpp>
#include <tyl/engine/core/drag_and_drop.hpp>
#include <tyl/engine/core/resources.hpp>
#include <tyl/engine/widgets/drag_and_drop.hpp>
#include <tyl/utility/entt.hpp>

namespace tyl::engine
{
namespace
{}  // namespace

class DragAndDrop::Impl
{
public:
  Impl() {}

  void update(Registry& registry, WidgetResources& resources)
  {
    if (!handle_load_error_popup(registry))
    {
      return;
    }
    else if (!handle_load_progress_popup(registry))
    {
      return;
    }

    if (auto* data_ptr = get_if<core::DragAndDropData>(registry, [](const auto& data) { return !data.paths.empty(); });
        data_ptr != nullptr)
    {
      for (const auto& file_path_name : data_ptr->paths)
      {
        // TODO(perf) do loading in another thread
        // TODO(qol) show loading progress bar
        if (const auto id_or_error = core::asset::load(resources, file_path_name); !id_or_error.has_value())
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
  }

private:
  bool handle_load_error_popup(entt::registry& registry)
  {
    if (!last_errors_.empty() && !ImGui::IsPopupOpen("#ResultDialogue"))
    {
      ImGui::OpenPopup("#ResultDialogue");
    }

    if (ImGui::BeginPopup("#ResultDialogue"))
    {
      const bool should_close = ImGui::Button("close");
      ImGui::Text("imports: (%d)", static_cast<int>(currently_loading_.size()));
      ImGui::Text("errors: (%d)", static_cast<int>(last_errors_.size()));
      for (const auto& error : last_errors_)
      {
        ImGui::Text("> %s", error.c_str());
      }

      if (should_close)
      {
        last_errors_.clear();
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
      return false;
    }
    else
    {
      return true;
    }
  }

  bool handle_load_progress_popup(entt::registry& registry)
  {
    if (!currently_loading_.empty() && !ImGui::IsPopupOpen("Loading Assets"))
    {
      ImGui::OpenPopup("Loading Assets");
    }

    if (ImGui::BeginPopupModal("Loading Assets"))
    {
      core::asset::Path* loaded_path = nullptr;
      std::size_t loaded_count = 0;
      for (const auto id : currently_loading_)
      {
        if (registry.any_of<core::asset::IsLoading>(id))
        {
          continue;
        }
        else
        {
          loaded_path = std::addressof(registry.get<core::asset::Path>(id));
          ++loaded_count;
        }
      }

      ImGui::ProgressBar(static_cast<float>(loaded_count) / static_cast<float>(currently_loading_.size()));
      if (loaded_path != nullptr)
      {
        ImGui::Text("%s", loaded_path->string().c_str());
      }

      if (loaded_count == currently_loading_.size())
      {
        for (const auto id : currently_loading_)
        {
          if (registry.any_of<core::asset::ErrorCode>(id))
          {
            const auto& [error, file_path_name] = registry.get<core::asset::ErrorCode, core::asset::Path>(id);
            std::ostringstream oss;
            oss << "Error loading [" << file_path_name << "]: " << error;
            last_errors_.emplace_back(oss.str());
          }
        }
        currently_loading_.clear();
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
      return false;
    }
    return true;
  }

  std::vector<std::string> last_errors_ = {};
  std::vector<entt::entity> currently_loading_ = {};
};

DragAndDrop::DragAndDrop(std::unique_ptr<Impl>&& impl) : impl_{std::move(impl)} {}

DragAndDrop::~DragAndDrop() = default;

expected<DragAndDrop, WidgetCreationError> DragAndDrop::CreateImpl(const Options& options)
{
  return DragAndDrop{std::make_unique<Impl>()};
}

WidgetStatus DragAndDrop::UpdateImpl(Registry& registry, WidgetResources& resources)
{
  ImGui::SetCurrentContext(resources.imgui_ctx);
  impl_->update(resources);
  return WidgetStatus::kOk;
}

}  // namespace tyl::engine
