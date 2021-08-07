/**
 * @copyright 2021-present Brian Cairl
 *
 * @file wave.hpp
 */
#pragma once

// C++ Standard Library
#include <algorithm>
#include <iosfwd>
#include <iterator>
#include <memory>
#include <numeric>

// Tyl
#include <tyl/common/bitops.hpp>
#include <tyl/wfc/typedefs.hpp>

namespace tyl::wfc
{

class Wave;

class WaveIterator;

class WaveDomainView
{
public:
  void set(const ElementID id) { first_[id / DomainChunkBits] |= bitmask<DomainChunkType>(id % DomainChunkBits); }

  void clear(const ElementID id) { first_[id / DomainChunkBits] &= ~bitmask<DomainChunkType>(id % DomainChunkBits); }

  void clear() { std::fill(first_, last_, 0); }

  bool any() const
  {
    return std::any_of(first_, last_, [](const DomainChunkType c) { return c != 0; });
  }

  bool is_set(const ElementID id) const
  {
    return first_[id / DomainChunkBits] & bitmask<DomainChunkType>(id % DomainChunkBits);
  }

  bool is_collapsed() const
  {
    return std::accumulate(first_, last_, 0, [](const std::size_t prev, const DomainChunkType c) -> std::size_t {
             return prev + static_cast<std::size_t>(is_one_hot(c));
           }) == 1;
  }

  unsigned count() const
  {
    return std::accumulate(
      first_, last_, 0, [](const int prev, const DomainChunkType c) -> int { return prev + count_bits_set(c); });
  }

  ElementID id() const
  {
    const auto* p = std::find_if(first_, last_, [](const auto c) -> bool { return c; });

    ElementID id = 0;
    // TODO(enhancement) optimized with one-hot lookup
    while (*p != bitmask<DomainChunkType>(id))
    {
      ++id;
    }

    return id;
  }

private:
  WaveDomainView(DomainChunkType* first, DomainChunkType* last) : first_{first}, last_{last} {};

  DomainChunkType* first_;
  DomainChunkType* last_;
  friend class Wave;
  friend class WaveIterator;
};

class ConstWaveDomainView
{
public:
  // TODO(enhancement) dedup WaveDomainView
  bool any() const
  {
    return std::any_of(first_, last_, [](const DomainChunkType c) { return c != 0; });
  }

  // TODO(enhancement) dedup WaveDomainView
  bool is_set(const ElementID id) const
  {
    return first_[id / DomainChunkBits] & bitmask<DomainChunkType>(id % DomainChunkBits);
  }

  // TODO(enhancement) dedup WaveDomainView
  bool is_collapsed() const
  {
    return std::accumulate(first_, last_, 0, [](const std::size_t prev, const DomainChunkType c) -> std::size_t {
             return prev + static_cast<std::size_t>(is_one_hot(c));
           }) == 1;
  }

  // TODO(enhancement) dedup WaveDomainView
  unsigned count() const
  {
    return std::accumulate(
      first_, last_, 0, [](const int prev, const DomainChunkType c) -> int { return prev + count_bits_set(c); });
  }

  // TODO(enhancement) dedup WaveDomainView
  ElementID id() const
  {
    const auto* p = std::find_if(first_, last_, [](const auto c) -> bool { return c; });
    ElementID id = 0;
    // TODO(enhancement) optimized with one-hot lookup
    while (*p != bitmask<DomainChunkType>(id))
    {
      ++id;
    }
    return id;
  }

private:
  ConstWaveDomainView(const DomainChunkType* first, const DomainChunkType* last) : first_{first}, last_{last} {};

  const DomainChunkType* first_;
  const DomainChunkType* last_;
  friend class Wave;
  friend class WaveIterator;
};

class WaveIterator
{
public:
  ConstWaveDomainView operator*() const { return ConstWaveDomainView{p_, p_ + domain_chunk_size_}; }

  WaveDomainView operator*() { return WaveDomainView{p_, p_ + domain_chunk_size_}; }

  WaveIterator& operator++()
  {
    p_ += domain_chunk_size_;
    return *this;
  }

  WaveIterator operator++(int)
  {
    const WaveIterator ret{*this};
    this->operator++();
    return ret;
  }

  bool operator==(const WaveIterator& other) const { return p_ == other.p_; }

  bool operator!=(const WaveIterator& other) const { return p_ != other.p_; }

private:
  WaveIterator(DomainChunkType* p, const std::size_t domain_chunk_size) : p_{p}, domain_chunk_size_{domain_chunk_size}
  {}

  DomainChunkType* p_;
  std::size_t domain_chunk_size_;

  friend class Wave;
};

class Wave
{
public:
  Wave(const std::size_t rows, const std::size_t cols, const std::size_t elements);

  WaveDomainView operator()(const std::size_t i, const std::size_t j)
  {
    auto* const p = data_.get() + (i * cols_ + j) * domain_chunk_len_;
    return WaveDomainView{p, p + domain_chunk_len_};
  }

  ConstWaveDomainView operator()(const std::size_t i, const std::size_t j) const
  {
    const auto* const p = data_.get() + (i * cols_ + j) * domain_chunk_len_;
    return ConstWaveDomainView{p, p + domain_chunk_len_};
  }

  void collapse(const std::size_t i, const std::size_t j, const ElementID id)
  {
    auto domain = (*this)(i, j);
    domain.clear();
    domain.set(id);
  }

  inline std::size_t size() const { return rows_ * cols_; }

  inline std::size_t chunks() const { return rows_ * cols_ * domain_chunk_len_; }

  inline std::size_t rows() const { return rows_; }

  inline std::size_t cols() const { return cols_; }

  inline WaveIterator begin() { return WaveIterator{data_.get(), domain_chunk_len_}; }

  inline WaveIterator end() { return WaveIterator{data_.get() + chunks(), 0}; }

private:
  static constexpr std::size_t get_domain_chunk_length(const std::size_t n_elements)
  {
    return (n_elements / DomainChunkBits) + ((n_elements % DomainChunkBits) ? 1 : 0);
  }

  std::size_t rows_;
  std::size_t cols_;
  std::size_t domain_chunk_len_;
  std::unique_ptr<DomainChunkType[]> data_;
};

}  // namespace tyl::wfc

namespace std
{

template <> struct iterator_traits<tyl::wfc::WaveIterator>
{
  using difference_type = std::ptrdiff_t;
  using value_type = tyl::wfc::DomainChunkType;
  using pointer = tyl::wfc::DomainChunkType*;
  using reference = tyl::wfc::DomainChunkType&;
  using iterator_category = std::bidirectional_iterator_tag;
};

}  // namespace std
