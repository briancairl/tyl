/**
 * @copyright 2022-present Brian Cairl
 *
 * @file bitfield.hpp
 */
#pragma once

// C++ Standard Library
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <type_traits>

// Tyl
#include <tyl/utility/bits.hpp>

namespace tyl
{

/**
 * @brief Extends bit-field structures with useful helper methods
 */
template <typename BitFieldT, typename BlockT, std::size_t BlockCount> struct bitfield
{
  static_assert(BlockCount > 0, "BlockCount == 0 does not make sense");
  static_assert(std::is_integral_v<BlockT>, "BlockT must be an integral type, e.g. std::uint64_t");

  /// Block type alias
  using block_type = BlockT;

  /// Number of blocks
  static constexpr std::size_t block_count = BlockCount;

  /**
   * @brief Sets all bits to zero
   */
  constexpr void reset() { std::memset(this, 0, sizeof(BlockT) * BlockCount); }

  /**
   * @brief Returns true if any bits are set
   */
  constexpr bool any() const
  {
    if constexpr (BlockCount == 1)
    {
      return bits::any(block<0>());
    }
    else
    {
      return std::any_of(data(), data() + BlockCount, [](const BlockT& b) { return bits::any(b); });
    }
  }

  /**
   * @brief Returns true if no bits are set
   */
  constexpr bool none() const
  {
    if constexpr (BlockCount == 1)
    {
      return bits::none(block<0>());
    }
    else
    {
      return std::all_of(data(), data() + BlockCount, [](const BlockT& b) { return bits::none(b); });
    }
  }

  /**
   * @brief Sets bit at \c P to a particular \c state, either <code>{true, false}</code>
   */
  template <std::size_t P> constexpr void set_to(bool state)
  {
    if (state)
    {
      bits::set(block<P>(), P % sizeof(BlockT));
    }
    else
    {
      bits::clear(block<P>(), P % sizeof(BlockT));
    }
  }

  /**
   * @copydoc set_to
   * @note Same as counterpart, but \c p is specified at runtime
   */
  constexpr void set_to(std::size_t p, bool state)
  {
    if (state)
    {
      bits::set(block(p), p % sizeof(BlockT));
    }
    else
    {
      bits::clear(block(p), p % sizeof(BlockT));
    }
  }

  /**
   * @brief Sets bit at \c P to \c true
   */
  template <std::size_t P> constexpr void set() { bits::set(block<P>(), P % sizeof(BlockT)); }

  /**
   * @copydoc set
   * @note Same as counterpart, but \c p is specified at runtime
   */
  constexpr void set(std::size_t p) { bits::set(block(p), p % sizeof(BlockT)); }

  /**
   * @brief Sets bit at \c P to \c false
   */
  template <std::size_t P> constexpr void clear() { bits::clear(block<P>(), P % sizeof(BlockT)); }

  /**
   * @copydoc clear
   * @note Same as counterpart, but \c p is specified at runtime
   */
  constexpr void clear(std::size_t p) { bits::clear(block(p), p % sizeof(BlockT)); }

  /**
   * @brief Performs bitwise OR on a bitfield, as if operating on a trivial bit-mask
   */
  BitFieldT operator|(const bitfield& other) const
  {
    BitFieldT retval{other.underlying()};
    if constexpr (BlockCount == 1)
    {
      retval.template block<0>() |= this->template block<0>();
    }
    else
    {
      for (std::size_t i = 0; i < BlockCount; ++i)
      {
        retval.data()[i] |= this->data()[i];
      }
    }
    return retval;
  }

  /**
   * @brief Performs bitwise AND on a bitfield, as if operating on a trivial bit-mask
   */
  BitFieldT operator&(const bitfield& other) const
  {
    BitFieldT retval{other.underlying()};
    if constexpr (BlockCount == 1)
    {
      retval.template block<0>() &= this->template block<0>();
    }
    else
    {
      for (std::size_t i = 0; i < BlockCount; ++i)
      {
        retval.data()[i] &= this->data()[i];
      }
    }
    return retval;
  }

  /**
   * @brief Returns \c true if all bits of two bitfield objects are the same
   */
  BitFieldT operator==(const bitfield& other) const
  {
    if constexpr (BlockCount == 1)
    {
      return other.template block<0>() == this->template block<0>();
    }
    else
    {
      return std::equal(this->data(), this->data() + BlockCount, other.data());
    }
  }

  /**
   * @brief Returns reference to underlying \c BitFieldT type
   */
  constexpr BitFieldT& underlying() { return reinterpret_cast<BitFieldT&>(*this); }

  /**
   * @brief Returns immutable reference to underlying \c BitFieldT type
   */
  constexpr const BitFieldT& underlying() const { return reinterpret_cast<const BitFieldT&>(*this); }

  /**
   * @brief Returns pointer to underlying \c BitFieldT type
   */
  template <std::size_t Offset = 0> constexpr BlockT* data()
  {
    static_assert(sizeof(BlockT) * BlockCount <= sizeof(BitFieldT));
    static_assert(alignof(BlockT) == alignof(BitFieldT));
    return reinterpret_cast<BlockT*>(this) + Offset;
  }

  /**
   * @brief Returns immutable pointer to underlying \c BitFieldT type
   */
  template <std::size_t Offset = 0> constexpr const BlockT* data() const
  {
    static_assert(sizeof(BlockT) * BlockCount <= sizeof(BitFieldT));
    static_assert(alignof(BlockT) == alignof(BitFieldT));
    return reinterpret_cast<const BlockT*>(this) + Offset;
  }

  /**
   * @brief Returns reference to block where bit at \c P exists
   */
  template <std::size_t P> constexpr BlockT& block() { return data()[P / sizeof(BlockT)]; }

  /**
   * @brief Returns immutable reference to block where bit at \c P exists
   */
  template <std::size_t P> constexpr const BlockT& block() const { return data()[P / sizeof(BlockT)]; }

  /**
   * @copydoc block
   * @note Same as counterpart, but \c p is specified at runtime
   */
  constexpr BlockT& block(std::size_t p) { return data()[p / sizeof(BlockT)]; }

  /**
   * @copydoc block
   * @note Same as counterpart, but \c p is specified at runtime
   */
  constexpr const BlockT& block(std::size_t p) const { return data()[p / sizeof(BlockT)]; }
};

}  // namespace tyl
