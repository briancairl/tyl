/**
 * @copyright 2020-present Brian Cairl
 *
 * @file shader.cpp
 */

// C++ Standard Library
#include <fstream>
#include <sstream>

// Art
#include <tyl/graphics/gl.hpp>
#include <tyl/graphics/shader.hpp>

namespace tyl::graphics
{

inline static GLuint to_gl_shader_code(const ShaderType shader_type)
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


inline static const char* to_gl_shader_str(const ShaderType shader_type)
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

inline static shader_id_t create_gl_shader_source(const ShaderType shader_type)
{
  return glCreateShader(to_gl_shader_code(shader_type));
}


inline static shader_id_t create_gl_shader() { return glCreateProgram(); }


static void
validate_gl_shader_compilation(const GLuint shader_id, const ShaderType shader_type, const std::string& code)
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
    throw ShaderSource::CompilationFailure{oss.str()};
  }
}


static void validate_gl_shader_linkage(const GLuint program_id)
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
    throw ShaderSource::LinkageFailure{oss.str()};
  }
}

ShaderSource::ShaderSource(std::string code, const ShaderType type) :
    shader_id_{create_gl_shader_source(type)},
    shader_type_{type}
{
  // Transfer source code
  const char* c_code = code.c_str();
  const GLint c_len = code.size();
  glShaderSource(*shader_id_, 1, &c_code, &c_len);

  // Compile component shader code and check for errors
  glCompileShader(*shader_id_);

  // Validate compilation
  validate_gl_shader_compilation(*shader_id_, type, code);
}

ShaderSource::ShaderSource(ShaderSource&& other) : shader_id_{other.shader_id_} { other.shader_id_.reset(); }

ShaderSource& ShaderSource::operator=(ShaderSource&& other)
{
  new (this) ShaderSource{std::move(other)};
  return *this;
}

ShaderSource ShaderSource::load_from_file(const char* filename, const std::string& premable, const ShaderType type)
{
  std::ifstream ifs{filename};
  if (ifs.is_open())
  {
    return ShaderSource{premable + std::string{std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()},
                        type};
  }
  else
  {
    std::ostringstream oss;
    oss << "Could not open shader source file " << filename;
    throw FileReadFailure{oss.str()};
  }
}


ShaderSource ShaderSource::load_from_file(const char* filename, const ShaderType type)
{
  return load_from_file(filename, get_shader_version_preamble(), type);
}


ShaderSource::~ShaderSource()
{
  if (shader_id_)
  {
    glDeleteShader(shader_id_.value());
  }
}

void ShaderHandle::bind() const { glUseProgram(shader_id_.value()); }

void ShaderHandle::unbind() const { glUseProgram(0); }

void ShaderHandle::setBool(const char* var_name, const bool value) const
{
  glUniform1i(glGetUniformLocation(shader_id_.value(), var_name), static_cast<GLint>(value));
}

void ShaderHandle::setInt(const char* var_name, const int value) const
{
  glUniform1i(glGetUniformLocation(shader_id_.value(), var_name), value);
}

void ShaderHandle::setFloat(const char* var_name, const float value) const
{
  glUniform1f(glGetUniformLocation(shader_id_.value(), var_name), value);
}

void ShaderHandle::setVec2(const char* var_name, const float* data) const
{
  glUniform2fv(glGetUniformLocation(shader_id_.value(), var_name), 1, data);
}

void ShaderHandle::setVec2(const char* var_name, const float x, const float y) const
{
  glUniform2f(glGetUniformLocation(shader_id_.value(), var_name), x, y);
}

void ShaderHandle::setVec3(const char* var_name, const float* data) const
{
  glUniform3fv(glGetUniformLocation(shader_id_.value(), var_name), 1, data);
}
void ShaderHandle::setVec3(const char* var_name, const float x, const float y, const float z) const
{
  glUniform3f(glGetUniformLocation(shader_id_.value(), var_name), x, y, z);
}

void ShaderHandle::setVec4(const char* var_name, const float* data) const
{
  glUniform4fv(glGetUniformLocation(shader_id_.value(), var_name), 1, data);
}

void ShaderHandle::setVec4(const char* var_name, const float x, const float y, const float z, const float w) const
{
  glUniform4f(glGetUniformLocation(shader_id_.value(), var_name), x, y, z, w);
}

void ShaderHandle::setMat2(const char* var_name, const float* data) const
{
  glUniformMatrix2fv(glGetUniformLocation(shader_id_.value(), var_name), 1, GL_FALSE, data);
}

void ShaderHandle::setMat3(const char* var_name, const float* data) const
{
  glUniformMatrix3fv(glGetUniformLocation(shader_id_.value(), var_name), 1, GL_FALSE, data);
}

void ShaderHandle::setMat4(const char* var_name, const float* data) const
{
  glUniformMatrix4fv(glGetUniformLocation(shader_id_.value(), var_name), 1, GL_FALSE, data);
}

Shader::Shader(ShaderSource&& vertex_source, ShaderSource&& fragment_source) : ShaderHandle{create_gl_shader()}
{
  glAttachShader(shader_id_.value(), vertex_source.get_id());
  glAttachShader(shader_id_.value(), fragment_source.get_id());

  // Link shader program components
  glLinkProgram(shader_id_.value());

  // Validate program linkage
  validate_gl_shader_linkage(shader_id_.value());

  // Detach all component shaders no longer in use
  glDetachShader(shader_id_.value(), vertex_source.get_id());
  glDetachShader(shader_id_.value(), fragment_source.get_id());
}

Shader::Shader(ShaderSource&& vertex_source, ShaderSource&& fragment_source, ShaderSource&& geometry_source) :
    ShaderHandle{create_gl_shader()}
{
  glAttachShader(shader_id_.value(), vertex_source.get_id());
  glAttachShader(shader_id_.value(), fragment_source.get_id());
  glAttachShader(shader_id_.value(), geometry_source.get_id());

  // Link shader program components
  glLinkProgram(shader_id_.value());

  // Validate program linkage
  validate_gl_shader_linkage(shader_id_.value());

  // Detach all component shaders no longer in use
  glDetachShader(shader_id_.value(), vertex_source.get_id());
  glDetachShader(shader_id_.value(), fragment_source.get_id());
  glDetachShader(shader_id_.value(), geometry_source.get_id());
}

Shader::Shader(Shader&& other) : ShaderHandle{other.shader_id_} { other.shader_id_.reset(); }

Shader& Shader::operator=(Shader&& other)
{
  new (this) Shader{std::move(other)};
  return *this;
}

Shader::~Shader()
{
  if (shader_id_)
  {
    glDeleteProgram(shader_id_.value());
  }
}

std::string get_shader_version_preamble()
{
  GLint major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);

  char buffer[1024];
  sprintf(buffer, "#version %d%d0\n\n", major, minor);
  return std::string{buffer};
}

}  // namespace tyl::graphics
