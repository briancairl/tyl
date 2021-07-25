/**
 * @copyright 2020-present Brian Cairl
 *
 * @file image.h
 */
#ifndef TYL_GRAPHICS_IMAGE_H
#define TYL_GRAPHICS_IMAGE_H

// C++ Standard Library
#include <cstdint>
#include <memory>
#include <type_traits>

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

  static Image load_from_file(const char* filename, const int force_channel_count = 0);

  inline int rows() const { return rows_; }

  inline int cols() const { return cols_; }

  inline int channels() const { return channels_; }

  inline std::uint8_t* data() { return data_; }

  inline std::uint8_t* const data() const { return data_; }

private:
  std::uint8_t* data_ = nullptr;
  int rows_ = 0;
  int cols_ = 0;
  int channels_ = 0;
};

}  // namespace tyl::graphics

#endif  // TYL_GRAPHICS_IMAGE_H
