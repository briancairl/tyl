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
#include <tyl/wfc/common.hpp>

namespace tyl::wfc::adjacency
{

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

static_assert((sizeof(StorageType) * 8UL) >= Direction::COUNT, "StorageType cannot fit all directions");

inline Direction opposite(const Direction side)
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

class Table
{
public:
  explicit Table(const std::size_t element_count);

  void reset();

  inline std::size_t size() const { return element_count_ * element_count_; }

  inline std::size_t element_count() const { return element_count_; }

  inline StorageType& operator()(const ElementID src_id, const ElementID dst_id)
  {
    return data_[offset(src_id, dst_id)];
  }

  inline const StorageType& operator()(const ElementID src_id, const ElementID dst_id) const
  {
    return data_[offset(src_id, dst_id)];
  }

  inline void allow(const ElementID src_id, const ElementID dst_id, const Direction direction)
  {
    bitops::set(Table::operator()(src_id, dst_id), direction);
  }

  inline void prevent(const ElementID src_id, const ElementID dst_id, const Direction direction)
  {
    bitops::clear(Table::operator()(src_id, dst_id), direction);
  }

  inline void allow_symmetric(const ElementID src_id, const ElementID dst_id, const Direction direction)
  {
    allow(src_id, dst_id, direction);
    allow(dst_id, src_id, opposite(direction));
  }

  inline void prevent_symmetric(const ElementID src_id, const ElementID dst_id, const Direction direction)
  {
    prevent(src_id, dst_id, direction);
    prevent(dst_id, src_id, opposite(direction));
  }

  inline bool is_allowed(const ElementID src_id, const ElementID dst_id, const Direction direction) const
  {
    return bitops::check(data_[offset(src_id, dst_id)], direction);
  }

  inline StorageType* begin() { return data_.get(); }

  inline StorageType* end() { return data_.get() + size(); }

  inline const StorageType* begin() const { return data_.get(); }

  inline const StorageType* end() const { return data_.get() + size(); }

private:
  inline std::size_t offset(const ElementID src_id, const ElementID dst_id) const
  {
    return src_id * element_count_ + dst_id;
  }

  /// Total number of elements to represent
  std::size_t element_count_;

  /// N x N matrix of adjacencies between elements
  std::unique_ptr<StorageType[]> data_;
};

}  // namespace tyl::wfc::adjacency
