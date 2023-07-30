/**
 * @copyright 2023-present Brian Cairl
 *
 * @file drag_and_drop_handler.cpp
 */

// C++ Standard Library

// Entt
#include <entt/entt.hpp>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

// Tyl
#include <tyl/engine/core/drag_and_drop.hpp>
#include <tyl/engine/core/resource.hpp>
#include <tyl/engine/widgets/drag_and_drop.hpp>
#include <tyl/utility/entt.hpp>

namespace tyl::engine::widgets
{
namespace
{}  // namespace

class DragAndDrop::Impl
{
public:
  Impl() {}

  void update(entt::registry& registry)
  {
    if (!last_errors_.empty() && !ImGui::IsPopupOpen("#ResultDialogue"))
    {
      ImGui::OpenPopup("#ResultDialogue");
    }

    if (ImGui::BeginPopup("#ResultDialogue"))
    {
      const bool should_close = ImGui::Button("close");
      ImGui::Text("imports: (%d)", static_cast<int>(last_imported_counter_));
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

      return;
    }
    else if (auto* data_ptr =
               get_if<core::DragAndDropData>(registry, [](const auto& data) { return !data.paths.empty(); });
             data_ptr != nullptr)
    {
      last_imported_counter_ = 0;
      for (const auto& file_path_name : data_ptr->paths)
      {
        // TODO(perf) do loading in another thread
        // TODO(qol) show loading progress bar
        if (const auto id_or_error = core::resource::create(registry, file_path_name); !id_or_error.has_value())
        {
          std::ostringstream oss;
          oss << "Error loading [" << file_path_name << "]: " << id_or_error.error();
          last_errors_.emplace_back(oss.str());
        }
        else
        {
          ++last_imported_counter_;
        }
      }
    }
  }

private:
  std::size_t last_imported_counter_ = 0;
  std::vector<std::string> last_errors_ = {};
};

DragAndDrop::~DragAndDrop() = default;

tyl::expected<DragAndDrop, DragAndDrop::OnCreateErrorCode> DragAndDrop::create(const Options& options)
{
  return DragAndDrop{std::make_unique<Impl>()};
}

DragAndDrop::DragAndDrop(std::unique_ptr<Impl>&& impl) : impl_{std::move(impl)} {}

void DragAndDrop::update(ImGuiContext* const imgui_ctx, entt::registry& reg)
{
  ImGui::SetCurrentContext(imgui_ctx);
  impl_->update(reg);
}

}  // namespace tyl::engine::widgets
