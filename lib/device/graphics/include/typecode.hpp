/**
 * @copyright 2020-present Brian Cairl
 *
 * @file typecode.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>

namespace tyl::device::graphics
{

enum class TypeCode
{
  Float32,
  Float64,
  SInt32,
  UInt32,
};

template <TypeCode C> constexpr std::size_t byte_count();

template <typename T> constexpr TypeCode typecode();

template <> constexpr TypeCode typecode<float>() { return TypeCode::Float32; };

template <> constexpr TypeCode typecode<double>() { return TypeCode::Float64; };

template <> constexpr TypeCode typecode<int>() { return TypeCode::SInt32; };

template <> constexpr TypeCode typecode<unsigned>() { return TypeCode::UInt32; };

}  // namespace tyl::device::graphics
