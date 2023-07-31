/**
 * @copyright 2023-present Brian Cairl
 *
 * @file non_blocking_future.hpp
 */
#pragma once

// C++ Standard Library
#include <atomic>
#include <mutex>
#include <optional>
#include <thread>
#include <type_traits>

// Tyl
#include <tyl/utility/expected.hpp>

namespace tyl::async
{

enum class non_blocking_future_error
{
  not_ready,
  retrieved
};

template <typename T> class non_blocking_promise;

template <typename T> class non_blocking_future
{
public:
  explicit non_blocking_future(std::shared_ptr<non_blocking_promise<T>> shared_state) :
      shared_state_{std::move(shared_state)} {};

  bool valid() { return shared_state_->valid(); }

  expected<T, non_blocking_future_error> get() { return shared_state_->get(); }

private:
  std::shared_ptr<non_blocking_promise<T>> shared_state_;
};

template <typename T> class non_blocking_promise : public std::enable_shared_from_this<non_blocking_promise<T>>
{
public:
  non_blocking_promise() : result_ready_flag_{false}, result_opt_{std::nullopt} {}

  void set_value(T&& result)
  {
    {
      std::lock_guard lock{result_mutex_};
      result_opt_.emplace(std::move(result));
    }
    result_ready_flag_ = true;
  }

  non_blocking_future<T> get_future() { return non_blocking_future{this->shared_from_this()}; }

private:
  friend non_blocking_future<T>;

  bool valid()
  {
    if (result_ready_flag_)
    {
      std::lock_guard lock{result_mutex_};
      return result_opt_.has_value();
    }
    else
    {
      return false;
    }
  }

  expected<T, non_blocking_future_error> get()
  {
    expected<T, non_blocking_future_error> result = unexpected{non_blocking_future_error::retrieved};
    if (!result_ready_flag_)
    {
      result = unexpected{non_blocking_future_error::not_ready};
    }
    else if (std::lock_guard lock{result_mutex_}; result_opt_.has_value())
    {
      result = std::move(result_opt_).value();
      result_opt_.reset();
    }
    return result;
  }

  std::mutex result_mutex_;
  std::atomic<bool> result_ready_flag_;
  std::optional<T> result_opt_;
};

template <> class non_blocking_promise<void> : public std::enable_shared_from_this<non_blocking_promise<void>>
{
public:
  non_blocking_promise() : result_ready_flag_{false}, result_set_{false} {}

  void set_value()
  {
    result_set_ = true;
    result_ready_flag_ = true;
  }

  non_blocking_future<void> get_future() { return non_blocking_future{this->shared_from_this()}; }

private:
  friend non_blocking_future<void>;

  bool valid() const { return result_ready_flag_ and result_set_; }

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

  std::atomic<bool> result_ready_flag_;
  std::atomic<bool> result_set_;
};


}  // namespace tyl::async