/**
 * @copyright 2023-present Brian Cairl
 *
 * @file non_blocking_future.hpp
 */
#pragma once

// C++ Standard Library
#include <atomic>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <type_traits>

// Tyl
#include <tyl/utility/expected.hpp>

namespace tyl::async
{

/**
 * @brief Error code enumeration used when accessing held value of not_blocking_future
 */
enum class non_blocking_future_error
{
  not_ready,
  retrieved
};

// Forward declaration
template <typename T> class non_blocking_promise;

/**
 * @brief Represents a future value to be computed, generally in an asynchronous manner
 *
 * @tparam T  held value type
 */
template <typename T> class non_blocking_future
{
public:
  non_blocking_future(non_blocking_future&&) = default;

  /**
   * @brief Returns true if value held by future is valid
   */
  bool valid() { return shared_state_->valid(); }

  /**
   * @brief Returns held value if valid, or error
   *
   * @see not_blocking_future_error
   */
  expected<T, non_blocking_future_error> get() { return shared_state_->get(); }

private:
  friend class non_blocking_promise<T>;

  /**
   * @brief Constructs non_blocking_future from non_blocking_promise
   *
   * @note only accessible by non_blocking_promise<T>
   */
  explicit non_blocking_future(std::shared_ptr<non_blocking_promise<T>> shared_state) :
      shared_state_{std::move(shared_state)} {};

  /// Shared work state (parent non_blocking_promise state)
  std::shared_ptr<non_blocking_promise<T>> shared_state_;
};

/**
 * @brief Represents the state of an eventual value, shared between execution contexts
 *
 * @tparam T  held value type
 */
template <typename T> class non_blocking_promise : public std::enable_shared_from_this<non_blocking_promise<T>>
{
public:
  /**
   * @brief Creates a non_blocking_promise with unfulfilled result value
   */
  non_blocking_promise() : result_ready_flag_{false}, result_opt_{std::nullopt} {}

  /**
   * @brief Sets computed value
   */
  void set_value(T&& result)
  {
    {
      std::lock_guard lock{result_mutex_};
      result_opt_.emplace(std::move(result));
    }
    result_ready_flag_ = true;
  }

  /**
   * @brief Returns handle to shared work state
   */
  non_blocking_future<T> get_future() { return non_blocking_future{this->shared_from_this()}; }

private:
  friend class non_blocking_future<T>;

  /**
   * @brief Returns true if result is ready such that <code>non_blocking_promise::get()</code> is valid
   */
  bool valid()
  {
    if (result_ready_flag_)
    {
      std::lock_guard lock{result_mutex_};
      return result_opt_.has_value();
    }
    return false;
  }

  /**
   * @brief Returns held value if valid, or error
   *
   * @see not_blocking_future_error
   */
  expected<T, non_blocking_future_error> get()
  {
    if (!result_ready_flag_)
    {
      return unexpected{non_blocking_future_error::not_ready};
    }
    else if (std::lock_guard lock{result_mutex_}; result_opt_.has_value())
    {
      auto result = std::move(result_opt_).value();
      result_opt_.reset();
      return expected<T, non_blocking_future_error>{std::move(result)};
    }
    else
    {
      return unexpected{non_blocking_future_error::retrieved};
    }
  }

  /// Protects shared result state between threads of execution
  std::mutex result_mutex_;

  /// Indicates if result is ready or not
  std::atomic<bool> result_ready_flag_;

  /// Held value
  std::optional<T> result_opt_;
};

template <> class non_blocking_promise<void> : public std::enable_shared_from_this<non_blocking_promise<void>>
{
public:
  /**
   * @brief Creates a non_blocking_promise with unfulfilled result value
   */
  non_blocking_promise() : result_ready_flag_{false}, result_set_{false} {}

  /**
   * @brief Indicate that work is complete (no value is set since <code>T == void</code>)
   */
  void set_value()
  {
    result_set_ = true;
    result_ready_flag_ = true;
  }

  /**
   * @brief Returns handle to shared work state
   */
  non_blocking_future<void> get_future() { return non_blocking_future{this->shared_from_this()}; }

private:
  friend non_blocking_future<void>;

  /**
   * @brief Returns true if result is ready such that <code>non_blocking_promise::get()</code> is valid
   */
  bool valid() const { return result_ready_flag_ and result_set_; }

  /**
   * @brief Returns no error indicator, or error
   *
   * @see not_blocking_future_error
   */
  expected<void, non_blocking_future_error> get()
  {
    expected<void, non_blocking_future_error> result;
    if (!result_ready_flag_)
    {
      result = unexpected{non_blocking_future_error::not_ready};
    }
    else if (!result_set_)
    {
      result = unexpected{non_blocking_future_error::retrieved};
    }
    else
    {
      result_set_ = false;
    }
    return result;
  }

  /// Indicates if result is ready or not
  std::atomic<bool> result_ready_flag_;

  /// Indicates if result has been checked with <code>non_blocking_promise::get()</code> or not
  std::atomic<bool> result_set_;
};


}  // namespace tyl::async