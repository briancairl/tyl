/**
 * @copyright 2020-present Brian Cairl
 *
 * @file texture.h
 */
#ifndef TYL_GRAPHICS_TEXTURE_H
#define TYL_GRAPHICS_TEXTURE_H

// C++ Standard Library
#include <cstdint>
#include <memory>
#include <optional>
#include <type_traits>


namespace tyl::graphics
{

class Texture;
class Image;
class ShaderHandle;


/// ID type used for textures, ideally identical to the graphics API ID
using texture_id_t = unsigned;


/**
 * @brief Lightweight texture interface, used to refer to a texture
 */
class TextureHandle
{
public:
  TextureHandle(TextureHandle&&) = default;
  TextureHandle(const TextureHandle&) = default;
  ~TextureHandle() = default;

  TextureHandle& operator=(TextureHandle&&) = default;

  /**
   * @brrief Binds texture to a working texture unit
   */
  void bind(const unsigned texture_index) const;

  /**
   * @brrief Returns unique ID associated with loaded texture
   */
  inline texture_id_t get_id() const { return *texture_id_; };

  template <typename ID_T>
  inline explicit TextureHandle(ID_T&& texture_id) : texture_id_{std::forward<ID_T>(texture_id)}
  {}

protected:
  std::optional<texture_id_t> texture_id_;
};


/**
 * @brief RAII wrapper around a texture resource
 *
 *        Creates and destroys texture through graphics API
 *
 * @warning Do not pass around Texture, use TextureHandle
 */
class Texture : public TextureHandle
{
public:
  struct Options
  {
    enum class ChannelMode
    {
      R,  //< Red (1-channel)
      RG,  //< Red-green (2-channel)
      RGB,  //< Red-green-blue (3-channel)
      RGBA,  //< Red-green-blue-alpha (4-channel)
    };

    enum class Wrapping
    {
      CLAMP_TO_BORDER,
      REPEAT
    };

    enum class Sampling
    {
      LINEAR,
      NEAREST
    };

    ChannelMode channel_mode;

    Wrapping u_wrapping;

    Wrapping v_wrapping;

    Sampling min_sampling;

    Sampling mag_sampling;

    inline Options(
      const ChannelMode _channel_mode,
      const Wrapping _u_wrapping = Wrapping::CLAMP_TO_BORDER,
      const Wrapping _v_wrapping = Wrapping::CLAMP_TO_BORDER,
      const Sampling _min_sampling = Sampling::NEAREST,
      const Sampling _mag_sampling = Sampling::NEAREST) :
        channel_mode{_channel_mode},
        u_wrapping{_u_wrapping},
        v_wrapping{_v_wrapping},
        min_sampling{_min_sampling},
        mag_sampling{_mag_sampling}
    {}
  };

  Texture(Texture&& other);
  Texture(
    const long int h,
    const long int w,
    const float* const data,
    const Options& options = Options::ChannelMode::R);
  Texture(
    const long int h,
    const long int w,
    const double* const data,
    const Options& options = Options::ChannelMode::R);
  Texture(
    const long int h,
    const long int w,
    const std::int8_t* const data,
    const Options& options = Options::ChannelMode::R);
  Texture(
    const long int h,
    const long int w,
    const std::uint8_t* const data,
    const Options& options = Options::ChannelMode::R);
  Texture(
    const long int h,
    const long int w,
    const std::int16_t* const data,
    const Options& options = Options::ChannelMode::R);
  Texture(
    const long int h,
    const long int w,
    const std::uint16_t* const data,
    const Options& options = Options::ChannelMode::R);
  Texture(
    const long int h,
    const long int w,
    const std::int32_t* const data,
    const Options& options = Options::ChannelMode::R);
  Texture(
    const long int h,
    const long int w,
    const std::uint32_t* const data,
    const Options& options = Options::ChannelMode::R);
  Texture(const Image& image);
  // TODO : others?

  ~Texture();

  Texture& operator=(Texture&&);

  inline TextureHandle get_handle() const { return TextureHandle{texture_id_}; };

private:
  Texture(const Texture&) = default;
};

namespace texture_from_merged_detail
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

}  // namespace texture_from_merged_detail


/**
 * @brief Creates a multi-channel texture from a single channel texture
 */
template <typename R_PtrT, typename G_PtrT>
inline Texture create_texture_from_merged(
  const int h,
  const int w,
  R_PtrT r,
  G_PtrT g,
  Texture::Options options = Texture::Options::ChannelMode::RG)
{
  static_assert(
    std::is_pointer<R_PtrT>() or std::is_same<R_PtrT, std::nullptr_t>(), "R_PtrT must be a pointer type (or nullptr_t");
  static_assert(
    std::is_pointer<G_PtrT>() or std::is_same<G_PtrT, std::nullptr_t>(), "G_PtrT must be a pointer type (or nullptr_t");

  using ValueT = std::remove_const_t<typename texture_from_merged_detail::common_value_type<R_PtrT, G_PtrT>::type>;

  std::unique_ptr<ValueT[]> data{new ValueT[h * w * 2]};
  const int last_i = h * w;
  for (int i = 0; i < last_i; ++i)
  {
    data.get()[2 * i + 0] = texture_from_merged_detail::get_value_at<ValueT>(r, i);
    data.get()[2 * i + 1] = texture_from_merged_detail::get_value_at<ValueT>(g, i);
  }

  options.channel_mode = Texture::Options::ChannelMode::RG;
  return Texture{h, w, data.get(), options};
}


/**
 * @copydoc create_texture_from_merged
 */
template <typename R_PtrT, typename G_PtrT, typename B_PtrT>
inline Texture create_texture_from_merged(
  const int h,
  const int w,
  R_PtrT r,
  G_PtrT g,
  B_PtrT b,
  Texture::Options options = Texture::Options::ChannelMode::RGB)
{
  static_assert(
    std::is_pointer<R_PtrT>() or std::is_same<R_PtrT, std::nullptr_t>(), "R_PtrT must be a pointer type (or nullptr_t");
  static_assert(
    std::is_pointer<G_PtrT>() or std::is_same<G_PtrT, std::nullptr_t>(), "G_PtrT must be a pointer type (or nullptr_t");
  static_assert(
    std::is_pointer<B_PtrT>() or std::is_same<B_PtrT, std::nullptr_t>(), "B_PtrT must be a pointer type (or nullptr_t");

  using ValueT =
    std::remove_const_t<typename texture_from_merged_detail::common_value_type<R_PtrT, G_PtrT, B_PtrT>::type>;

  std::unique_ptr<ValueT[]> data{new ValueT[h * w * 3]};
  const int last_i = h * w;
  for (int i = 0; i < last_i; ++i)
  {
    data.get()[3 * i + 0] = texture_from_merged_detail::get_value_at<ValueT>(r, i);
    data.get()[3 * i + 1] = texture_from_merged_detail::get_value_at<ValueT>(g, i);
    data.get()[3 * i + 2] = texture_from_merged_detail::get_value_at<ValueT>(b, i);
  }

  options.channel_mode = Texture::Options::ChannelMode::RGB;
  return Texture{h, w, data.get(), options};
}


/**
 * @copydoc create_texture_from_merged
 */
template <typename R_PtrT, typename G_PtrT, typename B_PtrT, typename A_PtrT>
inline Texture create_texture_from_merged(
  const int h,
  const int w,
  R_PtrT r,
  G_PtrT g,
  B_PtrT b,
  A_PtrT a,
  Texture::Options options = Texture::Options::ChannelMode::RGBA)
{
  static_assert(
    std::is_pointer<R_PtrT>() or std::is_same<R_PtrT, std::nullptr_t>(), "R_PtrT must be a pointer type (or nullptr_t");
  static_assert(
    std::is_pointer<G_PtrT>() or std::is_same<G_PtrT, std::nullptr_t>(), "G_PtrT must be a pointer type (or nullptr_t");
  static_assert(
    std::is_pointer<B_PtrT>() or std::is_same<B_PtrT, std::nullptr_t>(), "B_PtrT must be a pointer type (or nullptr_t");
  static_assert(
    std::is_pointer<A_PtrT>() or std::is_same<A_PtrT, std::nullptr_t>(), "A_PtrT must be a pointer type (or nullptr_t");

  using ValueT =
    std::remove_const_t<typename texture_from_merged_detail::common_value_type<R_PtrT, G_PtrT, B_PtrT, A_PtrT>::type>;

  std::unique_ptr<ValueT[]> data{new ValueT[h * w * 4]};
  const int last_i = h * w;
  for (int i = 0; i < last_i; ++i)
  {
    data.get()[4 * i + 0] = texture_from_merged_detail::get_value_at<ValueT>(r, i);
    data.get()[4 * i + 1] = texture_from_merged_detail::get_value_at<ValueT>(g, i);
    data.get()[4 * i + 2] = texture_from_merged_detail::get_value_at<ValueT>(b, i);
    data.get()[4 * i + 3] = texture_from_merged_detail::get_value_at<ValueT>(a, i);
  }

  options.channel_mode = Texture::Options::ChannelMode::RGBA;
  return Texture{h, w, data.get(), options};
}

}  // namespace tyl::graphics

#endif  // TYL_GRAPHICS_TEXTURE_H
