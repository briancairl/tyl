/**
 * @copyright 2021-present Brian Cairl
 *
 * @file bitops.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdlib>
#include <type_traits>

// Tyl
#include <tyl/assert.hpp>

namespace tyl
{

template <typename T> class RefCounted;

template <typename T> class Ref
{
  static_assert(!std::is_reference_v<T>, "Type <T> should not be a reference type");

public:
  using non_const_t = std::remove_const_t<T>;

  constexpr std::size_t use_count() const { return *use_count_ptr_; }

  constexpr non_const_t* ptr() { return value_ptr_; }

  constexpr const non_const_t* ptr() const { return value_ptr_; }

  constexpr non_const_t& value() { return *value_ptr_; }

  constexpr const non_const_t& value() const { return *value_ptr_; }

  constexpr operator non_const_t&() { return value(); }

  constexpr operator const non_const_t&() const { return value(); }

  constexpr non_const_t& operator*() { return value(); }

  constexpr const non_const_t& operator*() const { return value(); }

  constexpr non_const_t* operator->() { return ptr(); }

  constexpr const non_const_t* operator->() const { return ptr(); }

  ~Ref() { --(*use_count_ptr_); }

  Ref(const Ref& other) : value_ptr_{other.value_ptr_}, use_count_ptr_{other.use_count_ptr_} { ++(*use_count_ptr_); }

private:
  Ref(T* ptr, std::size_t* const use_count_ptr) : value_ptr_{ptr}, use_count_ptr_{use_count_ptr}
  {
    ++(*use_count_ptr_);
  }

  T* value_ptr_;

  std::size_t* use_count_ptr_;

  friend class RefCounted<T>;
};

template <typename LhsT, typename RhsT> constexpr bool operator==(const Ref<LhsT>& lhs, const Ref<RhsT>& rhs)
{
  static_assert(std::is_same<std::remove_const_t<LhsT>, std::remove_const_t<RhsT>>());
  return lhs.ptr() == rhs.ptr();
}

template <typename LhsT, typename RhsT> constexpr bool operator!=(const Ref<LhsT>& lhs, const Ref<RhsT>& rhs)
{
  static_assert(std::is_same<std::remove_const_t<LhsT>, std::remove_const_t<RhsT>>());
  return lhs.ptr() != rhs.ptr();
}

template <typename T> class RefCounted
{
  static_assert(!std::is_reference_v<T>, "Type <T> should not be a reference type");

public:
  using non_const_t = std::remove_const_t<T>;

  constexpr std::size_t use_count() const { return use_count_; }

  constexpr Ref<non_const_t> ref() { return Ref<non_const_t>{this}; }

  constexpr Ref<const non_const_t> ref() const { return Ref<const non_const_t>{this, std::addressof(use_count_)}; }

  ~RefCounted() { TYL_ASSERT_EQ(use_count_, 0); }

protected:
  RefCounted() = default;

private:
  std::size_t use_count_ = 0;
};

}  // namespace tyl
