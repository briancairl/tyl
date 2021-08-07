/**
 * @copyright 2021-present Brian Cairl
 *
 * @file adjacency.h
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <memory>

// Tyl
#include <tyl/common/bitops.hpp>
#include <tyl/wfc/typedefs.hpp>

namespace tyl::wfc
{

class Adjacency
{
public:
  using StorageType = std::uint8_t;

  enum Direction : unsigned
  {
    Up,
    Down,
    Left,
    Right,
    Above,
    Below,
    COUNT
  };

  static constexpr unsigned DirectionCount = Direction::COUNT;

  static inline Direction opposite(const Direction side)
  {
    switch (side)
    {
    case Direction::Up:
      return Direction::Down;
    case Direction::Down:
      return Direction::Up;
    case Direction::Left:
      return Direction::Right;
    case Direction::Right:
      return Direction::Left;
    case Direction::Above:
      return Direction::Below;
    case Direction::Below:
      return Direction::Above;
    default:
      break;
    }
    return Direction::COUNT;
  }

  inline void set(const Direction s) { data_ |= bitmask<StorageType>(s); }

  inline void clear(const Direction s) { data_ &= ~bitmask<StorageType>(s); }

  inline void clear() { data_ = 0; }

  inline bool any() const { return data_ != 0; }

  inline bool is_set(const Direction s) const { return data_ & bitmask<StorageType>(s); }

private:
  StorageType data_;
};


class AdjacencyTable
{
public:
  explicit AdjacencyTable(const std::size_t element_count);

  void clear();

  inline std::size_t size() const { return element_count_ * element_count_; }

  inline std::size_t count() const { return element_count_; }

  inline void allow(const ElementID src_id, const ElementID dst_id, const Adjacency::Direction direction)
  {
    data_[linear_index(src_id, dst_id)].set(direction);
    data_[linear_index(dst_id, src_id)].set(Adjacency::opposite(direction));
  }

  inline void disallow(const ElementID src_id, const ElementID dst_id, const Adjacency::Direction direction)
  {
    data_[linear_index(src_id, dst_id)].clear(direction);
    data_[linear_index(dst_id, src_id)].clear(Adjacency::opposite(direction));
  }

  inline bool is_allowed(const ElementID src_id, const ElementID dst_id, const Adjacency::Direction direction) const
  {
    return data_[linear_index(src_id, dst_id)].is_set(direction);
  }

private:
  inline std::size_t linear_index(const ElementID src_id, const ElementID dst_id) const
  {
    return src_id * element_count_ + dst_id;
  }

  /// Total number of elements to represent
  std::size_t element_count_;

  /// N x N matrix of adjacencies between elements
  std::unique_ptr<Adjacency[]> data_;
};

}  // namespace tyl::wfc
