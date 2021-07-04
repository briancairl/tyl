/**
 * @copyright 2020-present Brian Cairl
 *
 * @file shader.h
 */
#ifndef TYL_GRAPHICS_SHADER_H
#define TYL_GRAPHICS_SHADER_H

// C++ Standard Library
#include <cstdint>
#include <exception>
#include <optional>
#include <string>
#include <utility>

namespace tyl::graphics
{

class Shader;


/// ID type used for shaders, ideally identical to the graphics API ID
using shader_id_t = unsigned;


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
  /**
   * @brief Exception thrown when a Shader program fails to compile
   */
  class CompilationFailure final : public std::exception
  {
  public:
    template <typename DetailsT>
    explicit CompilationFailure(DetailsT&& details) : details_{std::forward<DetailsT>(details)}
    {}

    const char* what() const noexcept override { return details_.c_str(); }

  private:
    std::string details_;
  };

  /**
   * @brief Exception thrown when a Shader program fails to link
   */
  class LinkageFailure final : public std::exception
  {
  public:
    template <typename DetailsT> explicit LinkageFailure(DetailsT&& details) : details_{std::forward<DetailsT>(details)}
    {}

    const char* what() const noexcept override { return details_.c_str(); }

  private:
    std::string details_;
  };

  /**
   * @brief Exception thrown when a Shader program source cannot be loaded from disk
   */
  class FileReadFailure final : public std::exception
  {
  public:
    template <typename DetailsT>
    explicit FileReadFailure(DetailsT&& details) : details_{std::forward<DetailsT>(details)}
    {}

    const char* what() const noexcept override { return details_.c_str(); }

  private:
    std::string details_;
  };

  ShaderSource(std::string code, const ShaderType type);
  ShaderSource(ShaderSource&&);
  ~ShaderSource();

  ShaderSource& operator=(ShaderSource&& other);

  inline bool valid() const { return static_cast<bool>(shader_id_); }
  inline operator bool() const { return this->valid(); }
  inline shader_id_t get_id() const { return shader_id_.value(); };
  inline ShaderType get_type() const { return shader_type_; };

  /**
   * @brief Loads shader source code from a file
   */
  static ShaderSource load_from_file(const char* filename, const ShaderType type);

  /**
   * @brief Loads shader source code from a file
   */
  static ShaderSource load_from_file(const char* filename, const std::string& premable, const ShaderType type);

private:
  ShaderSource(const ShaderSource&) = default;
  inline explicit ShaderSource(const shader_id_t shader_id) : shader_id_{shader_id} {}

  ::std::optional<shader_id_t> shader_id_;
  ShaderType shader_type_;
};


/**
 * @brief Lightweight shader interface, used to refer to a shader
 */
class ShaderHandle
{
public:
  ShaderHandle(ShaderHandle&&) = default;
  ShaderHandle(const ShaderHandle&) = default;
  ~ShaderHandle() = default;

  ShaderHandle& operator=(ShaderHandle&& other) = default;

  void bind() const;
  void unbind() const;

  inline bool valid() const { return static_cast<bool>(shader_id_); }
  inline operator bool() const { return this->valid(); }
  inline shader_id_t get_id() const { return shader_id_.value(); };

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

protected:
  inline explicit ShaderHandle(std::optional<shader_id_t> shader_id) : shader_id_{shader_id} {}

  std::optional<shader_id_t> shader_id_;

  friend class Shader;
};


/**
 * @brief RAII wrapper around a shader resource
 *
 *        Creates and destroys shader through graphics API
 *
 * @warning Do not pass around Shader, use ShaderHandle
 */
class Shader : public ShaderHandle
{
public:
  Shader(Shader&& other);
  Shader(ShaderSource&& vertex_source, ShaderSource&& fragment_source);
  Shader(ShaderSource&& vertex_source, ShaderSource&& fragment_source, ShaderSource&& geometry_source);

  ~Shader();

  Shader& operator=(Shader&& other);

  inline ShaderHandle get_handle() const { return ShaderHandle{shader_id_}; };

private:
  Shader(const Shader&) = default;
};


/**
 * @brief Returns shader version string
 */
std::string get_shader_version_preamble();

}  // namespace tyl::graphics

#endif  // TYL_GRAPHICS_SHADER_H
