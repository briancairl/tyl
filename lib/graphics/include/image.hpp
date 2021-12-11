/**
 * @copyright 2020-present Brian Cairl
 *
 * @file image.h
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <memory>
#include <type_traits>

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/vec.hpp>

namespace tyl::graphics
{

/**
 * @brief Managed image memory
 */
class Image
{
public:
  Image() = default;
  Image(Image&&) = default;
  ~Image();

  Image& operator=(Image&& other) = default;

  static Image
  load_from_file(const char* filename, const int force_channel_count = 0, const bool flip_vertically = false);

  constexpr const Size2i& size() const { return size_; }

  constexpr int channels() const { return channels_; }

  constexpr std::uint8_t* data() { return data_; }

  constexpr std::uint8_t* const data() const { return data_; }

private:
  std::uint8_t* data_ = nullptr;
  Size2i size_ = Size2i{0, 0};
  int channels_ = 0;
};

}  // namespace tyl::graphics
