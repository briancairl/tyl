/**
 * @copyright 2021-present Brian Cairl
 *
 * @file bitops.hpp
 */
#pragma once

// C++ Standard Library
#include <limits>
#include <type_traits>
#include <utility>

namespace tyl::bits
{

/**
 * @brief Returns the number of bits occupied by a \c BlockT
 */
template <typename BlockT> static constexpr std::size_t size() { return sizeof(BlockT) * 8UL; }

/**
 * @brief Returns a \c BlockT filled with all zeros
 */
template <typename BlockT> static constexpr BlockT zero() { return 0; }

/**
 * @brief Returns a \c BlockT representing the number "1"
 */
template <typename BlockT> static constexpr BlockT one() { return static_cast<BlockT>(1); }

/**
 * @brief Returns a \c BlockT filled with all ones
 */
template <typename BlockT> static constexpr BlockT all() { return std::numeric_limits<BlockT>::max(); }

/**
 * @brief Returns the number of bits which will not fill one or more \c BlockT
 */
template <typename BlockT, std::size_t Bits> static constexpr std::size_t remaining_bits()
{
  return Bits % size<BlockT>();
}

/**
 * @brief Returns the number of bits which will not fill one or more \c BlockT
 */
template <typename BlockT> static constexpr std::size_t remaining_bits(const std::size_t bits)
{
  return bits % size<BlockT>();
}

/**
 * @brief Returns the number of whole \c BlockT which can support some number of bits
 */
template <typename BlockT, std::size_t Bits> static constexpr std::size_t whole_blocks()
{
  return Bits / size<BlockT>();
}

/**
 * @brief Returns the number of whole \c BlockT which can support some number of bits
 */
template <typename BlockT> static constexpr std::size_t whole_blocks(const std::size_t bits)
{
  return bits / size<BlockT>();
}

/**
 * @brief Returns the minimum number \c BlockT which can hold some number of bits
 */
template <typename BlockT, std::size_t Bits> static constexpr std::size_t min_blocks()
{
  return whole_blocks<BlockT, Bits>() + (static_cast<BlockT>(remaining_bits<BlockT, Bits>() > 0) * one<BlockT>());
}

/**
 * @brief Returns the minimum number \c BlockT which can hold some number of bits
 */
template <typename BlockT> static constexpr std::size_t min_blocks(const std::size_t bits)
{
  return whole_blocks<BlockT>(bits) + (static_cast<BlockT>(remaining_bits<BlockT>(bits) > 0) * one<BlockT>());
}

/**
 * @brief Returns a default \c BlockT given a boolean state, high or low
 */
template <typename BlockT> static constexpr BlockT default_block(const bool state)
{
  return static_cast<BlockT>(state) * all<BlockT>();
}

/**
 * @brief Returns a default \c BlockT given a boolean state, high or low
 */
template <typename BlockT, bool State> static constexpr BlockT default_block()
{
  return static_cast<BlockT>(State) * all<BlockT>();
}

/**
 * @brief Returns a default \c BlockT where the bit at \c offet is set high, and all others set to low
 */
template <typename BlockT> constexpr BlockT make_mask(const std::size_t offset) { return (one<BlockT>() << offset); }

/**
 * @brief Returns a default \c BlockT where the bit at \c Offset is set high, and all others set to low
 */
template <typename BlockT, std::size_t Offset> constexpr BlockT make_mask() { return (one<BlockT>() << Offset); }

/**
 * @brief Returns a default \c BlockT where the first-N bits are set high, and all others set to low
 */
template <typename BlockT> constexpr BlockT make_mask_first_n(const std::size_t n)
{
  return (std::numeric_limits<BlockT>::max() >> (sizeof(BlockT) * 8UL - n));
}

/**
 * @brief Returns a default \c BlockT where the first-N bits are set high, and all others set to low
 */
template <typename BlockT, std::size_t N> constexpr BlockT make_mask_first_n()
{
  return (std::numeric_limits<BlockT>::max() >> (sizeof(BlockT) * 8UL - N));
}

/**
 * @brief Returns \c true if one bit of a block is set
 */
template <typename BlockT> constexpr bool is_one_hot(BlockT m) { return m and !(m & (m - 1)); }

/**
 * @brief Returns the number of bits set in a block
 */
template <typename BlockT> constexpr BlockT count(const BlockT n) { return (n == 0) ? 0 : ((n & 1) + count(n >> 1)); }

/**
 * @brief Returns \c true if any bit it set
 */
template <typename BlockT> constexpr bool any(BlockT&& mask) { return std::forward<BlockT>(mask) != 0; }

/**
 * @brief Sets bit high in a \c BlockT block high at \c offset
 */
template <typename BlockT, typename IndexT> constexpr void set(BlockT&& mask, const IndexT offset)
{
  std::forward<BlockT>(mask) |= make_mask<std::remove_reference_t<BlockT>>(offset);
}

/**
 * @brief Clears bit (sets to low) in a \c BlockT block high at \c offset
 */
template <typename BlockT, typename IndexT> constexpr void clear(BlockT&& mask, const IndexT offset)
{
  std::forward<BlockT>(mask) &= ~make_mask<std::remove_reference_t<BlockT>>(offset);
}

/**
 * @brief Clears bit (sets to low) in a \c BlockT block high at \c offset
 */
template <typename BlockT, typename IndexT> constexpr void clear(BlockT&& mask) { std::forward<BlockT>(mask) = 0; }

/**
 * @brief Returns \c true if bit in \c BlockT at \c offset is set high
 */
template <typename BlockT, typename IndexT> constexpr bool check(BlockT&& mask, const IndexT offset)
{
  return std::forward<BlockT>(mask) & make_mask<std::remove_reference_t<BlockT>>(offset);
}

}  // namespace tyl::bits
