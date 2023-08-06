/**
 * @copyright 2023-present Brian Cairl
 *
 * @file synchronized.hpp
 */
#pragma once

// C++ Standard Library
#include <mutex>
#include <shared_mutex>
#include <type_traits>

namespace tyl::async
{

// Forward declaration
template <typename ValueT> class synchronized;

/**
 * @brief Provides access to a value under an active lock
 */
template <typename ValueT, typename LockT> class synchronized_handle
{
  static_assert(!std::is_reference_v<ValueT>, "'ValueT' must not be a reference type");

public:
  using value_type = ValueT;
  using const_value_type = const std::remove_const_t<value_type>;

  synchronized_handle(synchronized_handle&&) = default;

  /**
   * @brief Return mutable reference to value
   */
  constexpr value_type& get() { return *data_; }

  /**
   * @brief Return immutable reference to value
   */
  constexpr const_value_type& get() const { return *data_; }

  /**
   * @brief Assignment to value
   */
  template <typename AssigmentT> constexpr void set(AssigmentT&& other) { get() = std::forward<AssigmentT>(other); }

  /**
   * @copydoc set
   * @return *this
   */
  template <typename AssigmentT> constexpr synchronized_handle& operator=(AssigmentT&& other)
  {
    set(std::forward<AssigmentT>(other));
    return *this;
  }

  /**
   * @brief Return mutable reference to value
   */
  constexpr value_type& operator*() { return get(); }

  /**
   * @brief Return immutable reference to value
   */
  constexpr const_value_type& operator*() const { return get(); }

  /**
   * @brief Pointer like access to mutable value
   */
  constexpr value_type* operator->() { return data_; }

  /**
   * @brief Pointer like access to immutable value
   */
  constexpr const_value_type* operator->() const { return data_; }

private:
  template <typename _> friend class synchronized;

  /**
   * @brief Copying handle is not allowed
   */
  synchronized_handle(const synchronized_handle&) = delete;

  /**
   * @brief Only synchronized can create a synchronized_handle
   */
  template <typename M> synchronized_handle(M& mutex, ValueT& data) : lock_{mutex}, data_{std::addressof(data)} {}

  /// Active data lock
  LockT lock_;

  /// Reference to data whose access is synchronized under lock_
  ValueT* data_;
};

/**
 * @brief A value whose access is synchronized between threads of execution
 */
template <typename ValueT> class synchronized
{
public:
  /// Handle which allows shared access to a value
  using rlock_handle = synchronized_handle<const ValueT, std::shared_lock<std::shared_mutex>>;

  /// Handle which allows unique access to a value
  using wlock_handle = synchronized_handle<ValueT, std::unique_lock<std::shared_mutex>>;

  /**
   * @brief Constructs held value
   */
  template <typename... ArgTs>
  constexpr explicit synchronized(ArgTs&&... args) : data_mutex_{}, data_storage_{std::forward<ArgTs>(args)...}
  {}

  /**
   * @brief Returns handle to mutable value under lock
   */
  [[nodiscard]] auto wlock() { return wlock_handle{data_mutex_, data_storage_}; }

  /**
   * @brief Returns handle to immutable value under lock
   */
  [[nodiscard]] auto rlock() const { return rlock_handle{data_mutex_, data_storage_}; }

  /**
   * @brief Executes a unary function, passed the underlying value, under lock
   */
  template <typename UnaryFn> void with_lock(UnaryFn fn)
  {
    if constexpr (std::is_invocable<UnaryFn, const ValueT&>())
    {
      auto locked = rlock();
      fn(*locked);
    }
    else
    {
      auto locked = wlock();
      fn(*locked);
    }
  }

private:
  /// Mutex which protects access to data_storage_
  mutable std::shared_mutex data_mutex_;

  /// Synchronized data
  ValueT data_storage_;
};

template <typename ValueT> constexpr bool operator==(const synchronized<ValueT>& lhs, const synchronized<ValueT>& rhs)
{
  return (*lhs.rlock()) == (*rhs.rlock());
}

template <typename ValueT, typename ValueConvertibleT>
constexpr bool operator==(const synchronized<ValueT>& lhs, const ValueConvertibleT& rhs)
{
  static_assert(std::is_convertible_v<ValueConvertibleT, ValueT>);
  return (*lhs.rlock()) == rhs;
}

template <typename ValueConvertibleT, typename ValueT>
constexpr bool operator==(const ValueConvertibleT& lhs, const synchronized<ValueT>& rhs)
{
  static_assert(std::is_convertible_v<ValueConvertibleT, ValueT>);
  return lhs == (*rhs.rlock());
}

template <typename ValueT> constexpr bool operator!=(const synchronized<ValueT>& lhs, const synchronized<ValueT>& rhs)
{
  return (*lhs.rlock()) != (*rhs.rlock());
}

template <typename ValueT, typename ValueConvertibleT>
constexpr bool operator!=(const synchronized<ValueT>& lhs, const ValueConvertibleT rhs)
{
  static_assert(std::is_convertible_v<ValueConvertibleT, ValueT>);
  return (*lhs.rlock()) != rhs;
}

template <typename ValueConvertibleT, typename ValueT>
constexpr bool operator!=(const ValueConvertibleT& lhs, const synchronized<ValueT>& rhs)
{
  static_assert(std::is_convertible_v<ValueConvertibleT, ValueT>);
  return lhs != (*rhs.rlock());
}

}  // namespace tyl::async