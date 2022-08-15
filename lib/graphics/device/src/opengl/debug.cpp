/**
 * @copyright 2021-present Brian Cairl
 *
 * @file gl_error.cpp
 */

// C++ Standard Library
#include <cstdio>

// Tyl
#include <tyl/graphics/device/debug.hpp>
#include <tyl/graphics/device/gl.inl>

#warning "Compiling with graphical device (OpenGL) debugging enabled"

namespace tyl::graphics::device
{
namespace  // anonymous
{

const char* gl_debug_source_to_str(GLenum source)
{
  switch (source)
  {
  case GL_DEBUG_SOURCE_API:
    return "API";
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    return "WINDOW_SYSTEM";
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    return "SHADER_COMPILER";
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    return "THIRD_PARTY";
  case GL_DEBUG_SOURCE_APPLICATION:
    return "APPLICATION";
  case GL_DEBUG_SOURCE_OTHER:
    [[fallthrough]];
  default:
    break;
  }
  return "OTHER";
}

const char* gl_debug_severity_to_str(GLenum severity)
{
  switch (severity)
  {
  case GL_DEBUG_SEVERITY_HIGH:
    return "HIGH";
  case GL_DEBUG_SEVERITY_MEDIUM:
    return "MEDIUM";
  case GL_DEBUG_SEVERITY_LOW:
    return "LOW";
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    [[fallthrough]];
  default:
    break;
  }
  return "NOTIFICATION";
}

const char* gl_debug_type_to_str(GLenum type)
{
  switch (type)
  {
  case GL_DEBUG_TYPE_ERROR:
    return "ERROR";
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    return "DEPRECATED_BEHAVIOR";
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    return "UNDEFINED_BEHAVIOR";
  case GL_DEBUG_TYPE_PORTABILITY:
    return "PORTABILITY";
  case GL_DEBUG_TYPE_PERFORMANCE:
    return "PERFORMANCE";
  case GL_DEBUG_TYPE_MARKER:
    return "MARKER";
  case GL_DEBUG_TYPE_PUSH_GROUP:
    return "PUSH_GROUP";
  case GL_DEBUG_TYPE_POP_GROUP:
    return "POP_GROUP";
  case GL_DEBUG_TYPE_OTHER:
    [[fallthrough]];
  default:
    break;
  }
  return "OTHER";
}

void gl_debug_log_callback(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* user_data)
{
  // clang-format off
  if (type == GL_DEBUG_TYPE_ERROR)
  {
    std::fprintf(
      stderr,
      "[gl:error:%s:%s:%s:%d] %s\n",
      gl_debug_source_to_str(source),
      gl_debug_type_to_str(type),
      gl_debug_severity_to_str(severity),
      id,
      message);
  }
  else
  {
    std::fprintf(
      stderr,
      "[gl:debug:%s:%s:%s:%d] %s\n",
      gl_debug_source_to_str(source),
      gl_debug_type_to_str(type),
      gl_debug_severity_to_str(severity),
      id,
      message);
  }
  // clang-format on
}

void gl_error_log_callback(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* user_data)
{
  // clang-format off
  if (type != GL_DEBUG_TYPE_ERROR)
  {
    return;
  }

  std::fprintf(
    stderr,
    "[gl:error:%s:%s:%s:%d] %s\n",
    gl_debug_source_to_str(source),
    gl_debug_type_to_str(type),
    gl_debug_severity_to_str(severity),
    id,
    message);
  // clang-format on
}

}  // namespace anonymous

void enable_debug_logs()
{
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(gl_debug_log_callback, nullptr);
}

void disable_debug_logs() { glDisable(GL_DEBUG_OUTPUT); }

void enable_error_logs()
{
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(gl_error_log_callback, nullptr);
}

void disable_error_logs() { glDisable(GL_DEBUG_OUTPUT); }

}  // namespace tyl::graphics::device
