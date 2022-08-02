/**
 * @copyright 2022-present Brian Cairl
 *
 * @file allocator.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

namespace tyl
{

/**
 * @brief Global allocator type used for memory dynamically allocated by containers
 */
template <typename T> using ContainerAllocator = std::allocator<T>;

}  // namespace tyl
