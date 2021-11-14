/**
 * @copyright 2021-present Brian Cairl
 *
 * @file style.hpp
 */
#pragma once

// Forward declaration
class ImGuiStyle;

namespace tyl::ui
{

/**
 * @brief Custom ImGui style configuration, setup on initialization
 *
 *        Note that title-bar colors match window colors, intentionally, to prevent "highlighting" when windows
 *        are hovered/focused. This is because window focusing is used to make sure time-lines are draw over all
 *        plots
 */
void InitStyleDefault(ImGuiStyle* style);

}  // namespace tyl::ui
