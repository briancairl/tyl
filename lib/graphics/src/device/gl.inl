#ifdef TYL_GRAPHICS_GL_INL
#error("Detected double include of gl.inl")
#else
#define TYL_GRAPHICS_GL_INL
#endif  // TYL_GRAPHICS_GL_INL

// clang-format off

// OpenGL
#include <GL/gl3w.h>
#include <GL/gl.h>

// clang-format on

// C++ Standard Library
#include <type_traits>

// Tyl
#include <tyl/graphics/device/typecode.hpp>
#include <tyl/graphics/device/typedef.hpp>

namespace tyl::graphics::device
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

static constexpr GLboolean to_gl_bool(const bool value) { return value ? GL_TRUE : GL_FALSE; }

static_assert(std::is_same<GLint, int>());

static_assert(std::is_same<GLuint, shader_id_t>());

static_assert(std::is_same<GLuint, texture_id_t>());

static_assert(std::is_same<GLuint, vertex_buffer_id_t>());

}  // tyl::graphics::device
