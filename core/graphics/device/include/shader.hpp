/**
 * @copyright 2020-present Brian Cairl
 *
 * @file shader.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <memory>
#include <string_view>
#include <utility>

// Tyl
#include <tyl/expected.hpp>
#include <tyl/graphics/device/constants.hpp>
#include <tyl/graphics/device/fwd.hpp>
#include <tyl/graphics/device/typedef.hpp>

namespace tyl::graphics::device
{

/**
 * @brief Enumeration of shader types which make up a shader program
 */
enum class ShaderType
{
  VERTEX,
  FRAGMENT,
  GEOMETRY,
};

/**
 * @brief Shader source code
 */
class ShaderSource
{
public:
  /**
   * @brief Possible ShaderSource creation errors
   */
  enum class ErrorCode
  {
    LOAD_FAILURE,
    COMPILATION_FAILURE,
  };

  ShaderSource(ShaderSource&&);
  ~ShaderSource();

  ShaderSource& operator=(ShaderSource&&);

  /**
   * @brief Shader source ID
   */
  inline shader_id_t get_id() const noexcept { return shader_id_; };

  /**
   * @brief Shader type code
   */
  inline ShaderType get_type() const noexcept { return shader_type_; };

  /**
   * @brief Creates vertex shader source with detected graphics library version header
   */
  [[nodiscard]] static tyl::expected<ShaderSource, ShaderSource::ErrorCode>
  vertex(std::string_view code, std::string* const error_details = nullptr) noexcept;

  /**
   * @brief Creates fragment shader source with detected graphics library version header
   */
  [[nodiscard]] static tyl::expected<ShaderSource, ShaderSource::ErrorCode>
  fragment(std::string_view code, std::string* const error_details = nullptr) noexcept;

  /**
   * @brief Creates geometry shader source with detected graphics library version header
   */
  [[nodiscard]] static tyl::expected<ShaderSource, ShaderSource::ErrorCode>
  geometry(std::string_view code, std::string* const error_details = nullptr) noexcept;

  /**
   * @brief Creates a ShaderSource from code
   */
  [[nodiscard]] static tyl::expected<ShaderSource, ShaderSource::ErrorCode>
  create(std::string_view code, const ShaderType type, std::string* const error_details = nullptr) noexcept;

  /**
   * @brief Loads shader source code from a file
   */
  [[nodiscard]] static tyl::expected<ShaderSource, ShaderSource::ErrorCode>
  load_from_file(const char* filename, const ShaderType type, const bool fill_version_preamble = true) noexcept;

private:
  ShaderSource(std::string_view code, const ShaderType type);

  inline explicit ShaderSource(const shader_id_t shader_id) : shader_id_{shader_id} {}

  /// Indicates the device-side shader ID
  shader_id_t shader_id_;

  /// Indicates the type of shader
  ShaderType shader_type_;
};

/**
 * @brief Compiled shader program, downloaded to host
 */
struct ShaderProgramHost
{
public:
  [[nodiscard]] inline auto* data() { return data_.get(); }
  [[nodiscard]] inline const auto* data() const { return data_.get(); }
  [[nodiscard]] constexpr std::size_t size() const { return size_; }
  [[nodiscard]] constexpr enum_t format() const { return format_; }

  inline bool valid() const { return static_cast<bool>(data_); }

  ShaderProgramHost(std::unique_ptr<std::uint8_t>&& data, const std::size_t len, const enum_t format);

private:
  ShaderProgramHost() = default;

  /// Manages host-side compiled shader data
  std::unique_ptr<std::uint8_t> data_ = nullptr;

  /// Size of shader data, in bytes
  std::size_t size_ = 0;

  /// Shader format type code
  enum_t format_ = 0;

  friend class Shader;
};

/**
 * @brief RAII wrapper around a shader resource
 *
 *        Creates and destroys shader through graphics API
 *
 * @warning Do not pass around Shader, use ShaderHandle
 */
class Shader
{
public:
  /**
   * @brief Possible ShaderSource creation errors
   */
  enum class ErrorCode
  {
    LINKAGE_FAILURE,
  };

  Shader(Shader&& other);

  ~Shader();

  Shader& operator=(Shader&& other);

  [[nodiscard]] ShaderProgramHost download() const;

  void bind() const;
  void unbind() const;

  [[nodiscard]] inline bool valid() const { return shader_id_ != invalid_shader_id; }
  [[nodiscard]] inline operator bool() const { return Shader::valid(); }
  [[nodiscard]] inline shader_id_t get_id() const { return shader_id_; };

  void setBool(const char* var_name, const bool value) const;
  void setInt(const char* var_name, const int value) const;
  void setFloat(const char* var_name, const float value) const;
  void setVec2(const char* var_name, const float* data) const;
  void setVec2(const char* var_name, const float x, const float y) const;
  void setVec3(const char* var_name, const float* data) const;
  void setVec3(const char* var_name, const float x, const float y, const float z) const;
  void setVec4(const char* var_name, const float* data) const;
  void setVec4(const char* var_name, const float x, const float y, const float z, const float w) const;
  void setMat2(const char* var_name, const float* data) const;
  void setMat3(const char* var_name, const float* data) const;
  void setMat4(const char* var_name, const float* data) const;

  /**
   * @brief Creates a shader from vertex/fragment sources
   */
  [[nodiscard]] static tyl::expected<Shader, ErrorCode> create(
    const ShaderSource& vertex_source,
    const ShaderSource& fragment_source,
    std::string* const error_details = nullptr) noexcept;

  /**
   * @brief Creates a shader from vertex/fragment/geomtery sources
   */
  [[nodiscard]] static tyl::expected<Shader, ErrorCode> create(
    const ShaderSource& vertex_source,
    const ShaderSource& fragment_source,
    const ShaderSource& geometry_source,
    std::string* const error_details = nullptr) noexcept;

  /**
   * @brief Creates a binary shader blob managed by the host
   */
  [[nodiscard]] static tyl::expected<Shader, ErrorCode>
  create(const ShaderProgramHost& shader_host, std::string* const error_details = nullptr) noexcept;

private:
  inline explicit Shader(const shader_id_t shader_id) : shader_id_{shader_id} {}
  Shader(const ShaderSource& vertex_source, const ShaderSource& fragment_source);
  Shader(const ShaderSource& vertex_source, const ShaderSource& fragment_source, const ShaderSource& geometry_source);
  Shader(const ShaderProgramHost& shader_host);
  Shader(const Shader&) = default;

  /// Device shader ID
  shader_id_t shader_id_;
};

}  // namespace tyl::graphics::device
