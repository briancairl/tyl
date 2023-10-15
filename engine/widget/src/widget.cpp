// ImGui
#include <imgui.h>

// Tyl
#include <tyl/assert.hpp>
#include <tyl/engine/widget.hpp>

namespace tyl::engine
{

void WidgetUpdateCommon(Registry& registry, WidgetResources& resources) { TYL_ASSERT_NON_NULL(resources.gui_context); }

}  // namespace tyl::engine
