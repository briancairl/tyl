/**
 * @copyright 2021-present Brian Cairl
 *
 * @file typedef.hpp
 */
#pragma once

namespace tyl::graphics::device
{

/// Indexing type
using index_t = unsigned long;

/// ID type used for enumerations
using enum_t = unsigned;

/// ID type used for shaders, ideally identical to the graphics API ID
using shader_id_t = unsigned;

/// ID type used for textures, ideally identical to the graphics API ID
using texture_id_t = unsigned;

/// ID type used for frame buffer targets, ideally identical to the graphics API ID
using frame_buffer_id_t = unsigned;

/// ID type used for vertex_buffers, ideally identical to the graphics API ID
using vertex_buffer_id_t = unsigned;

/**
 * @brief RGBA color type
 */
struct Color
{
  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;
  float a = 0.0f;
};

/**
 * @brief 2D size type
 *
 * Used for 2D textures and viewport sizes
 */
struct Shape2D
{
  int height;
  int width;
};

}  // namespace tyl::graphics::device
