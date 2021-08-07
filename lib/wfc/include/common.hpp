/**
 * @copyright 2021-present Brian Cairl
 *
 * @file typedefs.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>

namespace tyl::wfc
{

using ElementID = std::size_t;

using DomainChunkType = std::uint8_t;

static constexpr unsigned DomainChunkBits = 8 * sizeof(DomainChunkType);

struct Loc
{
  std::size_t layer;
  std::size_t x;
  std::size_t y;
};

}  // namespace tyl::wfc
