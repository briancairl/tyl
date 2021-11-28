/**
 * @copyright 2021-present Brian Cairl
 *
 * @file texture.hpp
 */
#pragma once

// Tyl
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/fwd.hpp>
#include <tyl/ref.hpp>
#include <tyl/vec.hpp>

namespace tyl::graphics
{

/**
 * @brief Texture resource
 */
class Texture : public RefCounted<Texture>, public device::Texture
{
public:
  using ChannelMode = device::TextureChannelMode;

  using Options = device::TextureOptions;

  Texture(Texture&& other) = default;

  template <typename DataPtrT>
  Texture(
    const Size2i size,
    const DataPtrT* const data,
    const ChannelMode mode = ChannelMode::R,
    const Options& options = Options{}) :
      RefCounted<Texture>{},
      device::Texture{size.x(), size.y(), data, mode, options},
      size_{size}
  {}

  ~Texture() = default;

  inline const Size2i& size() const { return size_; }

private:
  Size2i size_;
};

/**
 * @brief Creates a Texture from Image data
 */
Texture to_texture(const Image& image, const Texture::Options& options = Texture::Options{});

namespace detail
{

/**
 * @brief Resolves a safe, common pointer and value type associated with a set of raw pointer types
 */
template <typename PtrT, typename... OtherPtrTs> struct common_value_type
{
  using ptr_t =
    std::conditional_t<std::is_pointer<PtrT>::value, PtrT, typename common_value_type<OtherPtrTs...>::ptr_t>;

  using type = std::remove_pointer_t<ptr_t>;

  template <typename TargetT, typename... ValueTs> struct check_value_type
  {
    static_assert(
      ((std::is_same<ValueTs, TargetT>() or std::is_same<ValueTs, std::nullptr_t>()) and ...),
      "Data pointer types do not match");
    static constexpr bool value = true;
  };

  static constexpr bool _ok = check_value_type<type*, PtrT, OtherPtrTs...>::value;
};

/**
 * @copydoc common_value_type
 * @note base case
 */
template <typename PtrT> struct common_value_type<PtrT>
{
  using ptr_t = std::remove_pointer_t<PtrT>;
};

/**
 * @brief Returns a value from a buffer (raw pointer), or zero if <code>PtrT == nullptr_t</code>
 */
template <typename ValueT, typename PtrT>
static inline ValueT get_value_at([[maybe_unused]] PtrT data, [[maybe_unused]] const int index)
{
  if constexpr (std::is_pointer<PtrT>::value)
  {
    return data[index];
  }
  else
  {
    return ValueT{0};
  }
}

}  // namespace detail


/**
 * @brief Creates a multi-channel texture from a single channel texture
 */
template <typename R_PtrT, typename G_PtrT>
inline Texture
merge_to_texture(const int h, const int w, R_PtrT r, G_PtrT g, const Texture::Options options = Texture::Options{})
{
  static_assert(
    std::is_pointer<R_PtrT>() or std::is_same<R_PtrT, std::nullptr_t>(), "R_PtrT must be a pointer type (or nullptr_t");
  static_assert(
    std::is_pointer<G_PtrT>() or std::is_same<G_PtrT, std::nullptr_t>(), "G_PtrT must be a pointer type (or nullptr_t");

  using ValueT = std::remove_const_t<typename detail::common_value_type<R_PtrT, G_PtrT>::type>;

  std::unique_ptr<ValueT[]> data{new ValueT[h * w * 2]};
  const int last_i = h * w;
  for (int i = 0; i < last_i; ++i)
  {
    data.get()[2 * i + 0] = detail::get_value_at<ValueT>(r, i);
    data.get()[2 * i + 1] = detail::get_value_at<ValueT>(g, i);
  }

  return Texture{h, w, data.get(), Texture::ChannelMode::RG, options};
}


/**
 * @copydoc merge_to_texture
 */
template <typename R_PtrT, typename G_PtrT, typename B_PtrT>
inline Texture merge_to_texture(
  const int h,
  const int w,
  R_PtrT r,
  G_PtrT g,
  B_PtrT b,
  const Texture::Options options = Texture::Options{})
{
  static_assert(
    std::is_pointer<R_PtrT>() or std::is_same<R_PtrT, std::nullptr_t>(), "R_PtrT must be a pointer type (or nullptr_t");
  static_assert(
    std::is_pointer<G_PtrT>() or std::is_same<G_PtrT, std::nullptr_t>(), "G_PtrT must be a pointer type (or nullptr_t");
  static_assert(
    std::is_pointer<B_PtrT>() or std::is_same<B_PtrT, std::nullptr_t>(), "B_PtrT must be a pointer type (or nullptr_t");

  using ValueT = std::remove_const_t<typename detail::common_value_type<R_PtrT, G_PtrT, B_PtrT>::type>;

  std::unique_ptr<ValueT[]> data{new ValueT[h * w * 3]};
  const int last_i = h * w;
  for (int i = 0; i < last_i; ++i)
  {
    data.get()[3 * i + 0] = detail::get_value_at<ValueT>(r, i);
    data.get()[3 * i + 1] = detail::get_value_at<ValueT>(g, i);
    data.get()[3 * i + 2] = detail::get_value_at<ValueT>(b, i);
  }

  return Texture{h, w, data.get(), Texture::ChannelMode::RGB, options};
}


/**
 * @copydoc merge_to_texture
 */
template <typename R_PtrT, typename G_PtrT, typename B_PtrT, typename A_PtrT>
inline Texture merge_to_texture(
  const int h,
  const int w,
  R_PtrT r,
  G_PtrT g,
  B_PtrT b,
  A_PtrT a,
  const Texture::Options options = Texture::Options{})
{
  static_assert(
    std::is_pointer<R_PtrT>() or std::is_same<R_PtrT, std::nullptr_t>(), "R_PtrT must be a pointer type (or nullptr_t");
  static_assert(
    std::is_pointer<G_PtrT>() or std::is_same<G_PtrT, std::nullptr_t>(), "G_PtrT must be a pointer type (or nullptr_t");
  static_assert(
    std::is_pointer<B_PtrT>() or std::is_same<B_PtrT, std::nullptr_t>(), "B_PtrT must be a pointer type (or nullptr_t");
  static_assert(
    std::is_pointer<A_PtrT>() or std::is_same<A_PtrT, std::nullptr_t>(), "A_PtrT must be a pointer type (or nullptr_t");

  using ValueT = std::remove_const_t<typename detail::common_value_type<R_PtrT, G_PtrT, B_PtrT, A_PtrT>::type>;

  std::unique_ptr<ValueT[]> data{new ValueT[h * w * 4]};
  const int last_i = h * w;
  for (int i = 0; i < last_i; ++i)
  {
    data.get()[4 * i + 0] = detail::get_value_at<ValueT>(r, i);
    data.get()[4 * i + 1] = detail::get_value_at<ValueT>(g, i);
    data.get()[4 * i + 2] = detail::get_value_at<ValueT>(b, i);
    data.get()[4 * i + 3] = detail::get_value_at<ValueT>(a, i);
  }

  return Texture{h, w, data.get(), Texture::ChannelMode::RGBA, options};
}

}  // namespace tyl::graphics
