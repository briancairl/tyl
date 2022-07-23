#ifdef TYL_GRAPHICS_GL_INL
#error("Detected double include of gl.inl")
#else
#define TYL_GRAPHICS_GL_INL
#endif  // TYL_GRAPHICS_GL_INL

// clang-format off

// GLAD
#include <glad/glad.h>

// GL
#include <GL/gl.h>

// clang-format on

// C++ Standard Library
#include <type_traits>

// Tyl
#include <tyl/device/graphics/typecode.hpp>
#include <tyl/device/graphics/typedef.hpp>

namespace tyl::device::graphics
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

template <> constexpr std::size_t byte_count<TypeCode::Float32>() { return 4UL; /* as per opengl specs */ }
static_assert(sizeof(float) == byte_count<TypeCode::Float32>());

template <> constexpr std::size_t byte_count<TypeCode::Float64>() { return 8UL;  /* as per opengl specs */ }
static_assert(sizeof(double) == byte_count<TypeCode::Float64>());

template <> constexpr std::size_t byte_count<TypeCode::SInt32>() { return 4UL;  /* as per opengl specs */ }
static_assert(sizeof(int) == byte_count<TypeCode::SInt32>());

template <> constexpr std::size_t byte_count<TypeCode::UInt32>() { return 4UL;  /* as per opengl specs */ }
static_assert(sizeof(unsigned) == byte_count<TypeCode::UInt32>());

inline std::size_t byte_count(const TypeCode code)
{
  switch (code)
  {
  case TypeCode::Float32:
    return byte_count<TypeCode::Float32>();
  case TypeCode::Float64:
    return byte_count<TypeCode::Float64>();
  case TypeCode::SInt32:
    return byte_count<TypeCode::SInt32>();
  case TypeCode::UInt32:
    return byte_count<TypeCode::UInt32>();
  default:
    break;
  }
  return 0UL;
}

}  // tyl::device::graphics
