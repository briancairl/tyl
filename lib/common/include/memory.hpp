/**
 * @copyright 2022-present Brian Cairl
 *
 * @file memory.hpp
 */
#pragma once

#ifdef TYL_ENABLED_TRACKED_ALLOCATION

// C++ Standard Library
#include <cstdint>
#include <memory>

namespace std
{

void* operator new(std::size_t size)
{
  cout << "New operator overloading " << endl;
  void* p = malloc(size);
  return p;
}

void operator delete(void* p)
{
  cout << "Delete operator overloading " << endl;
  free(p);
}

}  // namespace std

#endif  // TYL_ENABLED_TRACKED_ALLOCATION