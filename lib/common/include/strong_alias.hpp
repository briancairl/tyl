/**
 * @copyright 2021-present Brian Cairl
 *
 * @file strong_alias.hpp
 */
#pragma once

/**
 * @brief Creates a strong alias of a type
 */
#define TYL_DEFINE_STRONG_ALIAS(name, original_type)                                                                   \
  struct name : original_type                                                                                          \
  {                                                                                                                    \
    using original_type::original_type;                                                                                \
    using original_type::operator=;                                                                                    \
  };                                                                                                                   \
  static_assert(sizeof(name) == sizeof(original_type), "'sizeof aliasing type does not match sizeof aliased type");    \
  static_assert(alignof(name) == alignof(original_type), "'alignof aliasing type does not match alignof aliased type");
