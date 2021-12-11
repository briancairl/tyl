/**
 * @copyright 2021-present Brian Cairl
 *
 * @file filesystem.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>

#ifdef NDEBUG

#define TYL_ASSERT(cond, message) (void)0

#else

#define TYL_ASSERT(cond, message)                                                                                      \
  if (!(cond))                                                                                                         \
  {                                                                                                                    \
    std::printf(                                                                                                       \
      "\n***RUNTIME ASSERTION FAILED***\n\ncondition : %s\nmessage   : %s\nfile      : %s\nline      : %d\n",          \
      #cond,                                                                                                           \
      message,                                                                                                         \
      __FILE__,                                                                                                        \
      __LINE__);                                                                                                       \
    std::abort();                                                                                                      \
  }

#endif  // NDEBUG

#define TYL_ASSERT_NULL(val_ptr) TYL_ASSERT(val_ptr == nullptr, "expected pointer to have NULL value")

#define TYL_ASSERT_NON_NULL(val_ptr) TYL_ASSERT(val_ptr != nullptr, "expected pointer to have non-NULL value")

#define TYL_ASSERT_TRUE(val_bool) TYL_ASSERT(static_cast<bool>(val_bool), "expected expression to evaluate to TRUE")

#define TYL_ASSERT_FALSE(val_bool) TYL_ASSERT(!static_cast<bool>(val_bool), "expected expression to evaluate to FALSE")

#define TYL_ASSERT_EQ(val_lhs, val_rhs) TYL_ASSERT((val_lhs == val_rhs), "expected values to be equal")

#define TYL_ASSERT_NE(val_lhs, val_rhs) TYL_ASSERT((val_lhs != val_rhs), "expected values to be unequal")

#define TYL_ASSERT_LT(val_lhs, val_rhs)                                                                                \
  TYL_ASSERT((val_lhs < val_rhs), "expected left value to be less than right value")

#define TYL_ASSERT_LE(val_lhs, val_rhs)                                                                                \
  TYL_ASSERT((val_lhs <= val_rhs), "expected left value to be less or equal to than right value")

#define TYL_ASSERT_GT(val_lhs, val_rhs)                                                                                \
  TYL_ASSERT((val_lhs > val_rhs), "expected left value to be greater than right value")

#define TYL_ASSERT_GE(val_lhs, val_rhs)                                                                                \
  TYL_ASSERT((val_lhs >= val_rhs), "expected left value to be greater than or equal to right value")
