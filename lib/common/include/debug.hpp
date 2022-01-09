/**
 * @copyright 2022-present Brian Cairl
 *
 * @file debug.hpp
 */
#pragma once

#ifdef NDEBUG

#define TYL_DEBUG_SECTION(section) (void)0

#else

// C++ Standard Library
#include <iostream>

#define TYL_DEBUG_SECTION(section) section

#endif  // NDEBUG
