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
  _N,
};


/**
 * @brief Shader source code
 */
class ShaderSource
{
public:
  ShaderSource(ShaderSource&&);
  ShaderSource(std::string_view code, const ShaderType type);
  ~ShaderSource();

  ShaderSource& operator=(ShaderSource&&);

  /**
   * @brief Shader source ID
   */
  inline shader_id_t get_id() const { return shader_id_; };

  /**
   * @brief Shader type code
   */
  inline ShaderType get_type() const { return shader_type_; };

  /**
   * @brief Creates vertex shader source with detected graphics library version header
   */
  static ShaderSource vertex(std::string_view code);

  /**
   * @brief Creates fragment shader source with detected graphics library version header
   */
  static ShaderSource fragment(std::string_view code);

  /**
   * @brief Creates geometry shader source with detected graphics library version header
   */
  static ShaderSource geometry(std::string_view code);

  /**
   * @brief Loads shader source code from a file
   */
  static ShaderSource
  load_from_file(const char* filename, const ShaderType type, const bool fill_version_preamble = true);

private:
  ShaderSource(const ShaderSource&) = default;
  inline explicit ShaderSource(const shader_id_t shader_id) : shader_id_{shader_id} {}

  shader_id_t shader_id_;

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

  std::unique_ptr<std::uint8_t> data_ = nullptr;
  std::size_t size_ = 0;
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
  Shader(Shader&& other);
  Shader(const ShaderSource vertex_source, const ShaderSource fragment_source);
  Shader(const ShaderSource vertex_source, const ShaderSource fragment_source, const ShaderSource geometry_source);
  Shader(const ShaderProgramHost& shader_host);

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

private:
  inline explicit Shader(const shader_id_t shader_id) : shader_id_{shader_id} {}

  Shader(const Shader&) = default;

  /// Device shader ID
  shader_id_t shader_id_;
};

}  // namespace tyl::graphics::device
