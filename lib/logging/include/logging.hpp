/**
 * @copyright 2021-present Brian Cairl
 *
 * @file filesystem.hpp
 */
#pragma once

// SPDLOG
#include "spdlog/spdlog.h"

#ifndef TYL_DISBLE_LOGGING

#define TYL_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define TYL_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define TYL_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define TYL_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define TYL_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

#else  // TYL_DISBLE_LOGGING

#define TYL_TRACE(...)
#define TYL_INFO(...)
#define TYL_WARN(...)
#define TYL_DEBUG(...)
#define TYL_CRITICAL(...)

#endif  // TYL_DISBLE_LOGGING

namespace tyl::logging
{

/**
 * @brief Initializes logging system
 */
void initialize();

}  // namespace tyl::logging
