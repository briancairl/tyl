/**
 * @copyright 2022-present Brian Cairl
 *
 * @file dynamic_bitset.hpp
 */
#pragma once

// C++ Standard Library
#include <cstring>
#include <memory>
#include <numeric>
#include <type_traits>

// Tyl
#include <tyl/utility/bits.hpp>

namespace tyl
{

template <typename BlockT, typename Alloc = std::allocator<BlockT>> class dynamic_bitset
{
  static_assert(std::is_integral<BlockT>(), "Block type must be integral type");

public:
  static constexpr std::size_t bits_per_block = bits::size<BlockT>();

  // clang-format off
  using const_block_return_t = std::conditional_t<
    (sizeof(BlockT) < sizeof(BlockT&)),
    BlockT,
    const BlockT&
  >;
  // clang-format on

  dynamic_bitset() = default;

  explicit dynamic_bitset(const std::size_t bit_count)
  {
    dynamic_bitset::allocate(bits::min_blocks<BlockT>(bit_count));
    bit_count_ = bit_count;
  }

  dynamic_bitset(const std::size_t bit_count, const bool initial_state)
  {
    dynamic_bitset::allocate(bits::min_blocks<BlockT>(bit_count));
    bit_count_ = bit_count;
  }

  dynamic_bitset(dynamic_bitset&& other) :
      block_data_{other.block_data_},
      block_count_{other.block_count_},
      bit_count_{other.bit_count_},
      allocator_{other.allocator_}
  {
    other.block_data_ = nullptr;
  }

  ~dynamic_bitset() { dynamic_bitset::deallocate(); }

  void resize(const std::size_t bit_count, const bool state = false)
  {
    if (const std::size_t new_block_count = bits::min_blocks<BlockT>(bit_count); new_block_count > block_count_)
    {
      const std::size_t prev_block_count = block_count_;
      dynamic_bitset::reallocate(new_block_count);
      dynamic_bitset::fill(prev_block_count, block_count_, bits::default_block<BlockT>(state));
    }
    bit_count_ = bit_count;
  }

  void shrink_to_fit() { dynamic_bitset::reallocate(dynamic_bitset::min_block_count()); }

  void clear()
  {
    block_count_ = 0;
    bit_count_ = 0;
  }

  void release()
  {
    dynamic_bitset::deallocate();
    dynamic_bitset::clear();
    block_data_ = nullptr;
  }

  void fill(const bool state) { dynamic_bitset::fill(0, block_count_, bits::default_block<BlockT>(state)); }

  void set(const std::size_t bit) const
  {
    bits::set(block_data_[bits::whole_blocks<BlockT>(bit)], bits::remaining_bits<BlockT>(bit));
  }

  void flip(const std::size_t bit) const
  {
    bits::flip(block_data_[bits::whole_blocks<BlockT>(bit)], bits::remaining_bits<BlockT>(bit));
  }

  void clear(const std::size_t bit) const
  {
    bits::clear(block_data_[bits::whole_blocks<BlockT>(bit)], bits::remaining_bits<BlockT>(bit));
  }

  [[nodiscard]] constexpr bool test(const std::size_t bit) const
  {
    return bits::check(block_data_[bits::whole_blocks<BlockT>(bit)], bits::remaining_bits<BlockT>(bit));
  }

  [[nodiscard]] std::size_t count() const
  {
    if (block_data_ == nullptr)
    {
      return 0;
    }
    std::size_t bit_count = 0;
    for (std::size_t i = 0; i < block_count_; ++i)
    {
      bit_count += bits::count(block_data_[i]);
    }
    return bit_count;
  }

  [[nodiscard]] constexpr bool operator[](const std::size_t bit) const { return dynamic_bitset::test(bit); }

  [[nodiscard]] const_block_return_t block(const std::size_t block_index) const { return block_data_[block_index]; }

  [[nodiscard]] constexpr std::size_t size() const { return bit_count_; }

  [[nodiscard]] constexpr std::size_t min_block_count() { return bits::min_blocks<BlockT>(bit_count_); }

  [[nodiscard]] constexpr BlockT* block_data() { return block_data_; }

  [[nodiscard]] constexpr const BlockT* block_data() const { return block_data_; }

  [[nodiscard]] constexpr std::size_t blocks() const { return block_count_; }

private:
  void fill(const std::size_t first, const std::size_t last, const BlockT state)
  {
    for (std::size_t i = first; i < last; ++i)
    {
      block_data_[i] = state;
    }
  }

  void allocate(const std::size_t new_block_count)
  {
    auto* const new_block_data = allocator_.allocate(new_block_count);
    block_data_ = new_block_data;
    block_count_ = new_block_count;
  }

  void reallocate(const std::size_t new_block_count)
  {
    auto* const new_block_data = allocator_.allocate(new_block_count);
    if (block_data_ != nullptr)
    {
      std::memcpy(new_block_data, block_data_, sizeof(BlockT) * block_count_);
      allocator_.deallocate(block_data_, block_count_);
    }
    block_data_ = new_block_data;
    block_count_ = new_block_count;
  }

  void deallocate()
  {
    if (block_data_ == nullptr)
    {
      return;
    }
    allocator_.deallocate(block_data_, block_count_);
  }

  BlockT* block_data_ = nullptr;
  std::size_t block_count_ = 0;
  std::size_t bit_count_ = 0;
  Alloc allocator_;
};

template <typename BlockT, typename LAllocT, typename RAllocT>
constexpr bool operator==(const dynamic_bitset<BlockT, LAllocT>& lhs, const dynamic_bitset<BlockT, RAllocT>& rhs)
{
  return (lhs.size() == rhs.size()) and
    (std::memcmp(lhs.block_data(), rhs.block_data(), sizeof(BlockT) * lhs.blocks()) == 0);
}

template <typename BlockT, typename LAllocT, typename RAllocT>
constexpr bool operator!=(const dynamic_bitset<BlockT, LAllocT>& lhs, const dynamic_bitset<BlockT, RAllocT>& rhs)
{
  return !operator==(lhs, rhs);
}

}  // namespace tyl
