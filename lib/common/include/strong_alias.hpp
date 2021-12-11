/**
 * @copyright 2021-present Brian Cairl
 *
 * @file strong_alias.hpp
 */
#pragma once

#define TYL_ENSURE_LAYOUT_COMPAT(query, target)                                                                        \
  static_assert(sizeof(query) == sizeof(target), "'sizeof aliasing type does not match sizeof aliased type");          \
  static_assert(alignof(query) == alignof(target), "'alignof aliasing type does not match alignof aliased type");

/**
 * @brief Creates a strong alias of a type
 */
#define TYL_DEFINE_STRONG_ALIAS(name, original_type)                                                                   \
  struct name : original_type                                                                                          \
  {                                                                                                                    \
    using original_type::original_type;                                                                                \
    using original_type::operator=;                                                                                    \
  };                                                                                                                   \
  TYL_ENSURE_LAYOUT_COMPAT(name, original_type)
