/**
 * @copyright 2020-present Brian Cairl
 *
 * @file typecode.h
 */
#ifndef TYL_GRAPHICS_TYPECODE_H
#define TYL_GRAPHICS_TYPECODE_H

// C++ Standard Library
#include <cstdint>

namespace tyl::graphics
{

enum class TypeCode
{
  Float32,
  Float64,
  SInt32,
  UInt32,
};

inline std::size_t byte_count(const TypeCode code)
{
  switch (code)
  {
  case TypeCode::Float32:
    return sizeof(float);
  case TypeCode::Float64:
    return sizeof(double);
  case TypeCode::SInt32:
    return sizeof(std::int32_t);
  case TypeCode::UInt32:
    return sizeof(std::uint32_t);
  default:
    break;
  }
  return 0UL;
}

template <typename T> constexpr TypeCode type_to_code();

template <> constexpr TypeCode type_to_code<float>()
{
  static_assert(sizeof(float) == 4UL);
  return TypeCode::Float32;
};

template <> constexpr TypeCode type_to_code<double>()
{
  static_assert(sizeof(double) == 8UL);
  return TypeCode::Float64;
};

template <> constexpr TypeCode type_to_code<int>()
{
  static_assert(sizeof(int) == 4UL);
  return TypeCode::SInt32;
};

template <> constexpr TypeCode type_to_code<unsigned>()
{
  static_assert(sizeof(unsigned) == 4UL);
  return TypeCode::UInt32;
};

}  // arg::graphics

#endif  // TYL_GRAPHICS_TYPECODE_H
