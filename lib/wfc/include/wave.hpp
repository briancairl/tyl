/**
 * @copyright 2021-present Brian Cairl
 *
 * @file wave.hpp
 */
#pragma once

// C++ Standard Library
#include <algorithm>
#include <cmath>
#include <iosfwd>
#include <iterator>
#include <memory>
#include <numeric>

// Tyl
#include <tyl/common/array_view.hpp>
#include <tyl/common/bitops.hpp>
#include <tyl/wfc/common.hpp>

namespace tyl::wfc
{

class Wave;

class WaveIterator;

class ConstWaveDomainView
{
public:
  inline bool any() const
  {
    return std::any_of(first_, last_, [](const DomainChunkType c) { return c != 0; });
  }

  inline bool is_set(const ElementID id) const
  {
    return first_[id / DomainChunkBits] & bitops::make_mask<DomainChunkType>(id % DomainChunkBits);
  }

  inline bool is_collapsed() const
  {
    return std::accumulate(first_, last_, 0, [](const std::size_t prev, const DomainChunkType c) -> std::size_t {
             return prev + static_cast<std::size_t>(bitops::is_one_hot(c));
           }) == 1;
  }

  inline std::size_t count() const
  {
    return std::accumulate(
      first_, last_, 0, [](const int prev, const DomainChunkType c) -> int { return prev + bitops::count(c); });
  }

  inline ElementID id() const
  {
    ElementID id = 0;

    auto* const chunk_ptr = std::find_if(first_, last_, [](const auto c) -> bool { return c; });

    if (chunk_ptr == last_)
    {
      return -1;
    }

    const ElementID offset_id = id - (DomainChunkBits * std::distance(first_, chunk_ptr));

    while (*chunk_ptr != bitops::make_mask<DomainChunkType>(offset_id))
    {
      ++id;
    }

    return id;
  }

protected:
  ConstWaveDomainView(DomainChunkType* const first, DomainChunkType* const last) : first_{first}, last_{last} {};

  DomainChunkType* first_;
  DomainChunkType* last_;

private:
  ConstWaveDomainView(const DomainChunkType* const first, const DomainChunkType* const last) :
      ConstWaveDomainView{const_cast<DomainChunkType*>(first), const_cast<DomainChunkType*>(last)} {};

  friend class Wave;
  friend class WaveIterator;
};


class WaveDomainView : public ConstWaveDomainView
{
public:
  inline WaveDomainView& operator=(const ElementID id)
  {
    first_[id / DomainChunkBits] = bitops::make_mask<DomainChunkType>(id % DomainChunkBits);
    return *this;
  }

  inline void set(const ElementID id)
  {
    first_[id / DomainChunkBits] |= bitops::make_mask<DomainChunkType>(id % DomainChunkBits);
  }

  inline void clear(const ElementID id)
  {
    first_[id / DomainChunkBits] &= ~bitops::make_mask<DomainChunkType>(id % DomainChunkBits);
  }

  inline void clear() { std::fill(first_, last_, 0); }

private:
  WaveDomainView(DomainChunkType* const first, DomainChunkType* const last) : ConstWaveDomainView{first, last} {}

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
    WaveIterator::operator++();
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

struct WaveParameters
{
  std::size_t layer_count;
  std::size_t row_count;
  std::size_t col_count;
  ArrayView<const float> element_probabilities;
};

class Wave
{
public:
  Wave(const WaveParameters& parameters);

  void reset(const ArrayView<const float>& element_probabilities);

  inline WaveDomainView operator()(const std::size_t layer, const std::size_t i, const std::size_t j)
  {
    auto* const p = state_chunk_ptr(layer, i, j);
    return WaveDomainView{p, p + domain_chunk_len_};
  }

  inline ConstWaveDomainView operator()(const std::size_t layer, const std::size_t i, const std::size_t j) const
  {
    const auto* const p = state_chunk_ptr(layer, i, j);
    return ConstWaveDomainView{p, p + domain_chunk_len_};
  }

  inline std::size_t possible_states(const std::size_t layer, const std::size_t i, const std::size_t j) const
  {
    return count_[offset(layer, i, j)];
  }

  inline float entropy(const std::size_t layer, const std::size_t i, const std::size_t j) const
  {
    const std::size_t index = offset(layer, i, j);
    return (p_sum_[index] * std::log2(p_sum_[index]) - p_log_p_sum_[index]) / p_sum_[index];
  }

  inline void collapse(const std::size_t layer, const std::size_t i, const std::size_t j, const ElementID id)
  {
    // Set state to final collapsed (one-hot) value
    Wave::operator()(layer, i, j) = id;

    // Set supporting propbability data to reflect collapsation
    const std::size_t index = offset(layer, i, j);
    count_[index] = 1UL;
    p_sum_[index] = p_[id];
    p_log_p_sum_[index] = p_log_p_[id];
  }

  inline bool is_collapsed(const std::size_t layer, const std::size_t i, const std::size_t j) const
  {
    return count_[offset(layer, i, j)] == 1UL;
  }

  inline void eliminate(const std::size_t layer, const std::size_t i, const std::size_t j, const ElementID id)
  {
    // Unset element from state
    Wave::operator()(layer, i, j).clear(id);

    // Reduce entropy
    const std::size_t index = offset(layer, i, j);
    count_[index]--;
    p_sum_[index] -= p_[id];
    p_log_p_sum_[index] -= p_log_p_[id];
  }

  inline WaveDomainView operator()(const Loc& loc) { return Wave::operator()(loc.layer, loc.x, loc.y); }

  inline ConstWaveDomainView operator()(const Loc& loc) const { return Wave::operator()(loc.layer, loc.x, loc.y); }

  inline std::size_t possible_states(const Loc& loc) { return Wave::possible_states(loc.layer, loc.x, loc.y); }

  inline float entropy(const Loc& loc) { return Wave::entropy(loc.layer, loc.x, loc.y); }

  inline void collapse(const Loc& loc, const ElementID id) { Wave::collapse(loc.layer, loc.x, loc.y, id); }

  inline bool is_collapsed(const Loc& loc) const { return Wave::is_collapsed(loc.layer, loc.x, loc.y); }

  inline void eliminate(const Loc& loc, const ElementID id) { Wave::eliminate(loc.layer, loc.x, loc.y, id); }

  inline std::size_t elements() const { return element_count_; }

  inline std::size_t size() const { return layer_count_ * row_count_ * col_count_; }

  inline std::size_t chunks() const { return size() * domain_chunk_len_; }

  inline std::size_t layers() const { return layer_count_; }

  inline std::size_t rows() const { return row_count_; }

  inline std::size_t cols() const { return col_count_; }

  inline WaveIterator begin() { return WaveIterator{state_.get(), domain_chunk_len_}; }

  inline WaveIterator end() { return WaveIterator{state_.get() + chunks(), 0}; }

private:
  void reset_cell_state();

  void reset_cell_probabilities(const ArrayView<const float>& element_probabilities);

  inline std::size_t offset(const std::size_t layer, const std::size_t i, const std::size_t j) const
  {
    return (layer * row_count_ + i) * col_count_ + j;
  }

  inline DomainChunkType* state_chunk_ptr(const std::size_t layer, const std::size_t i, const std::size_t j) const
  {
    return state_.get() + offset(layer, i, j) * domain_chunk_len_;
  }

  std::size_t layer_count_;

  std::size_t row_count_;

  std::size_t col_count_;

  std::size_t element_count_;

  std::size_t domain_chunk_len_;

  std::unique_ptr<DomainChunkType[]> state_;

  std::unique_ptr<std::size_t[]> count_;

  std::unique_ptr<float[]> p_;

  std::unique_ptr<float[]> p_sum_;

  std::unique_ptr<float[]> p_log_p_;

  std::unique_ptr<float[]> p_log_p_sum_;

  friend std::ostream& operator<<(std::ostream& os, const Wave& wave);
};

}  // namespace tyl::wfc

namespace std
{

template <> struct iterator_traits<tyl::wfc::WaveIterator>
{
  using difference_type = std::ptrdiff_t;
  using value_type = ::tyl::wfc::DomainChunkType;
  using pointer = ::tyl::wfc::DomainChunkType*;
  using reference = ::tyl::wfc::DomainChunkType&;
  using iterator_category = std::bidirectional_iterator_tag;
};

}  // namespace std
