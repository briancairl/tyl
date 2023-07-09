/**
 * @copyright 2023-present Brian Cairl
 *
 * @file target.hpp
 */
#pragma once

// C++ Standard Library
#include <optional>

// Tyl
#include <tyl/graphics/device/fwd.hpp>
#include <tyl/graphics/device/typedef.hpp>
#include <tyl/utility/expected.hpp>

namespace tyl::graphics::device
{

class Target
{
public:
  /**
   * @brief Possible Target creation errors
   */
  enum class ErrorCode
  {
    INVALID_TARGET,
    INVALID_RENDER_TEXTURE,
    INVALID_MAX_HEIGHT,
    INVALID_MAX_WIDTH,
  };

  /**
   * @brief Target creation options
   */
  struct Options
  {
    bool enable_depth_testing = false;
  };

  static expected<Target, ErrorCode>
  create(const int max_height, const int max_width, const Options& options = {.enable_depth_testing = false});

  static expected<Target, ErrorCode>
  create(const TextureHandle& render_texture, const Options& options = {.enable_depth_testing = false});

  void set_max_extents(const int max_height, const int max_width)
  {
    max_height_ = max_height;
    max_width_ = max_width;
  }

  void bind(const int height, const int width) const;

  void bind() const;

  Target(Target&& other);

  ~Target();

private:
  Target(const Target&) = delete;
  Target(
    const int max_height,
    const int max_width,
    const target_id_t target_id,
    const std::optional<target_id_t> depth_target_id);

  int max_height_;
  int max_width_;
  target_id_t target_id_;
  std::optional<target_id_t> depth_target_id_;
};

}  // namespace tyl::graphics::device
