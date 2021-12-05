/**
 * @copyright 2021-present Brian Cairl
 *
 * @file debug.hpp
 */
#pragma once

namespace tyl::graphics::device
{

/**
 * @brief Enables global graphics all debug logs
 */
void enable_debug_logs();

/**
 * @brief Disables global graphics all debug logs
 */
void disable_debug_logs();

/**
 * @brief Enables global graphics error logs, only
 */
void enable_error_logs();

/**
 * @brief Disables global graphics error logs, only
 */
void disable_error_logs();

}  // namespace tyl::graphics::device
