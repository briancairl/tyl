/**
 * @copyright 2022-present Brian Cairl
 *
 * @file mem_istream.cpp
 */

// C++ Standard Library
#include <utility>

// Tyl
#include <tyl/serialization/mem_istream.hpp>
#include <tyl/serialization/mem_ostream.hpp>

namespace tyl::serialization
{

mem_istream::mem_istream(std::vector<std::uint8_t>&& buffer) : buffer_{std::move(buffer)} {}

mem_istream::mem_istream(mem_istream&& other) : buffer_{std::move(other.buffer_)} {}

mem_istream::mem_istream(mem_ostream&& other) : buffer_{std::move(other.buffer_)} {}

mem_istream::~mem_istream() = default;

}  // namespace tyl::serialization