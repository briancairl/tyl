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

/// ID type used for render targets, ideally identical to the graphics API ID
using target_id_t = unsigned;

/// ID type used for vertex_buffers, ideally identical to the graphics API ID
using vertex_buffer_id_t = unsigned;

}  // namespace tyl::graphics::device
