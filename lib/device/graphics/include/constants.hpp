/**
 * @copyright 2021-present Brian Cairl
 *
 * @file constants.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>

// Tyl
#include <tyl/device/graphics/typedef.hpp>

namespace tyl::device::graphics
{

/// Total number of available texture units
static constexpr std::size_t texture_unit_count = 16UL;

/// Invalid (NULL-like) shader ID value
static constexpr shader_id_t invalid_shader_id = 0;

/// Invalid (NULL-like) texture ID value
static constexpr texture_id_t invalid_texture_id = 0;

/// Invalid (NULL-like) vertex ID value
static constexpr vertex_buffer_id_t invalid_vertex_buffer_id = 0;

}  // namespace tyl::device::graphics
