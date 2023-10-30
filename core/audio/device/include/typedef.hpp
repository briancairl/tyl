/**
 * @copyright 2022-present Brian Cairl
 *
 * @file typedef.hpp
 */
#pragma once

namespace tyl::audio::device
{

/// Handle type used for context backed
using context_handle_t = void*;

/// Handle type used for sound playback devices
using device_handle_t = void*;

/// Handle type used for sound sinks (a.k.a listeners)
using listener_handle_t = void*;

/// Handle type used for sound sources
using source_handle_t = unsigned;

/// Handle type used for sound buffers
using buffer_handle_t = unsigned;


static constexpr context_handle_t kInvalidContextHandle = nullptr;
static constexpr device_handle_t kInvalidDeviceHandle = nullptr;
static constexpr listener_handle_t kInvalidListenerHandle = nullptr;
static constexpr source_handle_t kInvalidSourceHandle = 0;
static constexpr buffer_handle_t kInvalidBufferHandle = 0;

}  // namespace tyl::audio::device
