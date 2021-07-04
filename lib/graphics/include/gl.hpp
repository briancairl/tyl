#ifdef TYL_GRAPHICS_GL_H
#error("Detected double include of gl.h")
#else
#define TYL_GRAPHICS_GL_H
#endif  // TYL_GRAPHICS_GL_H

// clang-format off

// OpenGL
#include <GL/gl3w.h>
#include <GL/gl.h>

// clang-format on

// Art
#include <tyl/graphics/typecode.hpp>

namespace tyl::graphics
{

static inline GLuint to_gl_typecode(const TypeCode code)
{
  switch (code)
  {
  case TypeCode::Float32:
    return GL_FLOAT;
  case TypeCode::Float64:
    return GL_DOUBLE;
  case TypeCode::SInt32:
    return GL_INT;
  case TypeCode::UInt32:
    return GL_UNSIGNED_INT;
  default:
    break;
  }
  return GL_FLOAT;
}

static inline GLboolean to_gl_bool(const bool value) { return value ? GL_TRUE : GL_FALSE; }

}  // arg::graphics
