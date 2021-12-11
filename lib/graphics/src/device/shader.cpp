/**
 * @copyright 2020-present Brian Cairl
 *
 * @file shader.cpp
 */

// C++ Standard Library
#include <algorithm>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>

// Tyl
#include <tyl/assert.hpp>
#include <tyl/graphics/device/gl.inl>
#include <tyl/graphics/device/shader.hpp>

namespace tyl::graphics::device
{
namespace  // anonymous
{

/**
 * @brief Exception thrown when a Shader program fails to compile
 */
class GLShaderCompilationFailure final : public std::exception
{
public:
  template <typename DetailsT>
  explicit GLShaderCompilationFailure(DetailsT&& details) : details_{std::forward<DetailsT>(details)}
  {}

  const char* what() const noexcept override { return details_.c_str(); }

private:
  std::string details_;
};

/**
 * @brief Exception thrown when a Shader program fails to link
 */
class GLShaderLinkageFailure final : public std::exception
{
public:
  template <typename DetailsT>
  explicit GLShaderLinkageFailure(DetailsT&& details) : details_{std::forward<DetailsT>(details)}
  {}

  const char* what() const noexcept override { return details_.c_str(); }

private:
  std::string details_;
};

/**
 * @brief Exception thrown when a Shader program source cannot be loaded from disk
 */
class ShaderFileReadFailure final : public std::exception
{
public:
  template <typename DetailsT>
  explicit ShaderFileReadFailure(DetailsT&& details) : details_{std::forward<DetailsT>(details)}
  {}

  const char* what() const noexcept override { return details_.c_str(); }

private:
  std::string details_;
};

inline GLuint to_gl_shader_code(const ShaderType shader_type)
{
  switch (shader_type)
  {
  case ShaderType::VERTEX:
    return GL_VERTEX_SHADER;
  case ShaderType::FRAGMENT:
    return GL_FRAGMENT_SHADER;
  case ShaderType::GEOMETRY:
    return GL_GEOMETRY_SHADER;
  default:
    break;
  }
  return -1;
}


inline const char* to_gl_shader_str(const ShaderType shader_type)
{
  switch (shader_type)
  {
  case ShaderType::VERTEX:
    return "GL_VERTEX_SHADER";
  case ShaderType::FRAGMENT:
    return "GL_FRAGMENT_SHADER";
  case ShaderType::GEOMETRY:
    return "GL_GEOMETRY_SHADER";
  default:
    break;
  }
  return "ShaderType[INVALID]";
}

inline shader_id_t create_gl_shader_source(const ShaderType shader_type)
{
  return glCreateShader(to_gl_shader_code(shader_type));
}


inline shader_id_t create_gl_shader() { return glCreateProgram(); }


void validate_gl_shader_compilation(const GLuint shader_id, const ShaderType shader_type)
{
  // Check compilation status
  GLint success;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

  // Compiled correctly; nothing to do
  if (success == GL_TRUE)
  {
    return;
  }
  // Get shader compilation error
  else
  {
    GLint len;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &len);

    std::basic_string<GLchar> info_log_contents(static_cast<size_t>(len), '*');
    GLsizei written;
    glGetShaderInfoLog(shader_id, len, &written, info_log_contents.data());

    std::ostringstream oss;
    oss << "glCompileShader [" << to_gl_shader_str(shader_type) << ':' << shader_id << "]\n\n(" << written
        << " char)\n\n"
        << info_log_contents;
    throw GLShaderCompilationFailure{oss.str()};
  }
}


void validate_gl_shader_linkage(const GLuint program_id)
{
  // Check compilation status
  GLint success;
  glGetProgramiv(program_id, GL_LINK_STATUS, &success);

  // Compiled correctly; nothing to do
  if (success == GL_TRUE)
  {
    return;
  }
  // Get shader compilation error
  else
  {
    GLint len;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &len);

    std::basic_string<GLchar> info_log_contents(static_cast<size_t>(len), '*');
    GLsizei written;
    glGetProgramInfoLog(program_id, len, &written, info_log_contents.data());

    std::ostringstream oss;
    oss << "glLinkProgram [" << program_id << "]\n\n(" << written << " char)\n\n" << info_log_contents;
    throw GLShaderLinkageFailure{oss.str()};
  }
}

void put_shader_version_preamble(std::ostream& os)
{
  GLint major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  os << "#version " << major << minor << 0 << "\n\n";
}

}  // namespace anonymous

ShaderSource::ShaderSource(std::string_view code, const ShaderType type) :
    shader_id_{create_gl_shader_source(type)},
    shader_type_{type}
{
  TYL_ASSERT_NE(shader_id_, invalid_shader_id);

  // Transfer source code
  const char* c_code = code.data();
  const GLint c_len = code.size();
  glShaderSource(shader_id_, 1, &c_code, &c_len);

  // Compile component shader code and check for errors
  glCompileShader(shader_id_);

  // Validate compilation
  validate_gl_shader_compilation(shader_id_, type);
}

ShaderSource::ShaderSource(ShaderSource&& other) : shader_id_{other.shader_id_}
{
  other.shader_id_ = invalid_shader_id;
}

ShaderSource& ShaderSource::operator=(ShaderSource&& other)
{
  new (this) ShaderSource{std::move(other)};
  return *this;
}

ShaderSource ShaderSource::vertex(std::string_view code)
{
  std::ostringstream oss;
  put_shader_version_preamble(oss);
  oss << code;
  return ShaderSource{oss.str(), ShaderType::VERTEX};
}

ShaderSource ShaderSource::fragment(std::string_view code)
{
  std::ostringstream oss;
  put_shader_version_preamble(oss);
  oss << code;
  return ShaderSource{oss.str(), ShaderType::FRAGMENT};
}

ShaderSource ShaderSource::geometry(std::string_view code)
{
  std::ostringstream oss;
  put_shader_version_preamble(oss);
  oss << code;
  return ShaderSource{oss.str(), ShaderType::GEOMETRY};
}

ShaderSource ShaderSource::load_from_file(const char* filename, const ShaderType type, const bool fill_version_preamble)
{
  if (std::ifstream ifs{filename}; ifs.is_open())
  {
    std::ostringstream oss;

    // Add detected version preamble
    if (fill_version_preamble)
    {
      put_shader_version_preamble(oss);
    }

    // Add code from file
    std::copy(
      std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), std::ostreambuf_iterator<char>(oss));

    return ShaderSource{oss.str(), type};
  }
  else
  {
    std::ostringstream oss;
    oss << "Could not open shader source file " << filename;
    throw ShaderFileReadFailure{oss.str()};
  }
}

ShaderSource::~ShaderSource()
{
  if (shader_id_)
  {
    glDeleteShader(shader_id_);
  }
}

Shader::Shader(ShaderSource&& vertex_source, ShaderSource&& fragment_source) : Shader{create_gl_shader()}
{
  glAttachShader(shader_id_, vertex_source.get_id());
  glAttachShader(shader_id_, fragment_source.get_id());

  // Link shader program components
  glLinkProgram(shader_id_);

  // Validate program linkage
  validate_gl_shader_linkage(shader_id_);

  // Detach all component shaders no longer in use
  glDetachShader(shader_id_, vertex_source.get_id());
  glDetachShader(shader_id_, fragment_source.get_id());
}

Shader::Shader(ShaderSource&& vertex_source, ShaderSource&& fragment_source, ShaderSource&& geometry_source) :
    Shader{create_gl_shader()}
{
  glAttachShader(shader_id_, vertex_source.get_id());
  glAttachShader(shader_id_, fragment_source.get_id());
  glAttachShader(shader_id_, geometry_source.get_id());

  // Link shader program components
  glLinkProgram(shader_id_);

  // Validate program linkage
  validate_gl_shader_linkage(shader_id_);

  // Detach all component shaders no longer in use
  glDetachShader(shader_id_, vertex_source.get_id());
  glDetachShader(shader_id_, fragment_source.get_id());
  glDetachShader(shader_id_, geometry_source.get_id());
}

Shader::Shader(Shader&& other) : Shader{other.shader_id_} { other.shader_id_ = invalid_shader_id; }

Shader& Shader::operator=(Shader&& other)
{
  new (this) Shader{std::move(other)};
  return *this;
}

Shader::~Shader()
{
  if (Shader::valid())
  {
    glDeleteProgram(shader_id_);
  }
}

void Shader::bind() const
{
  TYL_ASSERT_NE(shader_id_, invalid_shader_id);
  glUseProgram(shader_id_);
}

void Shader::unbind() const { glUseProgram(invalid_shader_id); }

void Shader::setBool(const char* var_name, const bool value) const
{
  TYL_ASSERT_NE(shader_id_, invalid_shader_id);
  glUniform1i(glGetUniformLocation(shader_id_, var_name), static_cast<GLint>(value));
}

void Shader::setInt(const char* var_name, const int value) const
{
  TYL_ASSERT_NE(shader_id_, invalid_shader_id);
  glUniform1i(glGetUniformLocation(shader_id_, var_name), value);
}

void Shader::setFloat(const char* var_name, const float value) const
{
  TYL_ASSERT_NE(shader_id_, invalid_shader_id);
  glUniform1f(glGetUniformLocation(shader_id_, var_name), value);
}

void Shader::setVec2(const char* var_name, const float* data) const
{
  TYL_ASSERT_NE(shader_id_, invalid_shader_id);
  glUniform2fv(glGetUniformLocation(shader_id_, var_name), 1, data);
}

void Shader::setVec2(const char* var_name, const float x, const float y) const
{
  TYL_ASSERT_NE(shader_id_, invalid_shader_id);
  glUniform2f(glGetUniformLocation(shader_id_, var_name), x, y);
}

void Shader::setVec3(const char* var_name, const float* data) const
{
  TYL_ASSERT_NE(shader_id_, invalid_shader_id);
  glUniform3fv(glGetUniformLocation(shader_id_, var_name), 1, data);
}
void Shader::setVec3(const char* var_name, const float x, const float y, const float z) const
{
  TYL_ASSERT_NE(shader_id_, invalid_shader_id);
  glUniform3f(glGetUniformLocation(shader_id_, var_name), x, y, z);
}

void Shader::setVec4(const char* var_name, const float* data) const
{
  TYL_ASSERT_NE(shader_id_, invalid_shader_id);
  glUniform4fv(glGetUniformLocation(shader_id_, var_name), 1, data);
}

void Shader::setVec4(const char* var_name, const float x, const float y, const float z, const float w) const
{
  TYL_ASSERT_NE(shader_id_, invalid_shader_id);
  glUniform4f(glGetUniformLocation(shader_id_, var_name), x, y, z, w);
}

void Shader::setMat2(const char* var_name, const float* data) const
{
  TYL_ASSERT_NE(shader_id_, invalid_shader_id);
  glUniformMatrix2fv(glGetUniformLocation(shader_id_, var_name), 1, GL_FALSE, data);
}

void Shader::setMat3(const char* var_name, const float* data) const
{
  TYL_ASSERT_NE(shader_id_, invalid_shader_id);
  glUniformMatrix3fv(glGetUniformLocation(shader_id_, var_name), 1, GL_FALSE, data);
}

void Shader::setMat4(const char* var_name, const float* data) const
{
  TYL_ASSERT_NE(shader_id_, invalid_shader_id);
  glUniformMatrix4fv(glGetUniformLocation(shader_id_, var_name), 1, GL_FALSE, data);
}


}  // namespace tyl::graphics::device
