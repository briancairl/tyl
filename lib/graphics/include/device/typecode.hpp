/**
 * @copyright 2020-present Brian Cairl
 *
 * @file typecode.h
 */
#pragma once

// C++ Standard Library
#include <cstdint>

namespace tyl::graphics::device
{

enum class TypeCode
{
  Float32,
  Float64,
  SInt32,
  UInt32,
};

template <TypeCode C> constexpr std::size_t byte_count();

template <> constexpr std::size_t byte_count<TypeCode::Float32>() { return 4UL; }

template <> constexpr std::size_t byte_count<TypeCode::Float64>() { return 8UL; }

template <> constexpr std::size_t byte_count<TypeCode::SInt32>() { return 4UL; }

template <> constexpr std::size_t byte_count<TypeCode::UInt32>() { return 4UL; }

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

template <typename T> constexpr TypeCode type_to_code();

template <> constexpr TypeCode type_to_code<float>()
{
  static_assert(sizeof(float) == byte_count<TypeCode::Float32>());
  return TypeCode::Float32;
};

template <> constexpr TypeCode type_to_code<double>()
{
  static_assert(sizeof(double) == byte_count<TypeCode::Float64>());
  return TypeCode::Float64;
};

template <> constexpr TypeCode type_to_code<int>()
{
  static_assert(sizeof(int) == byte_count<TypeCode::SInt32>());
  return TypeCode::SInt32;
};

template <> constexpr TypeCode type_to_code<unsigned>()
{
  static_assert(sizeof(unsigned) == byte_count<TypeCode::UInt32>());
  return TypeCode::UInt32;
};

}  // namespace tyl::graphics::device
