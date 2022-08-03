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
  SInt16,
  UInt16,
  SInt8,
  UInt8,
};

template <TypeCode C> constexpr std::size_t byte_count();

template <typename T> constexpr TypeCode typecode();

template <> constexpr TypeCode typecode<float>() { return TypeCode::Float32; };

template <> constexpr TypeCode typecode<double>() { return TypeCode::Float64; };

template <> constexpr TypeCode typecode<std::int32_t>() { return TypeCode::SInt32; };

template <> constexpr TypeCode typecode<std::uint32_t>() { return TypeCode::UInt32; };

template <> constexpr TypeCode typecode<std::int16_t>() { return TypeCode::SInt16; };

template <> constexpr TypeCode typecode<std::uint16_t>() { return TypeCode::UInt16; };

template <> constexpr TypeCode typecode<std::int8_t>() { return TypeCode::SInt8; };

template <> constexpr TypeCode typecode<std::uint8_t>() { return TypeCode::UInt8; };

}  // namespace tyl::device::graphics
