/**
 * @copyright 2023-present Brian Cairl
 *
 * @file worker_pool.hpp
 */
#pragma once

// C++ Standard Library
#include <algorithm>
#include <array>
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

// Tyl
#include <tyl/async/non_blocking_future.hpp>

namespace tyl::async
{

/**
 * @brief Managers N threads of execution, decided at compile-time, each of which run an identical work-loop
 *
 * Joins threads on destruction
 *
 * @tparam N  thread count
 */
template <std::size_t N> class work_group
{
public:
  /**
   * @brief Starts all workers running work callback \c f
   */
  template <typename WorkLoopFnT> explicit work_group(WorkLoopFnT f)
  {
    work_group::each([f](auto& t) { t = std::thread{f}; });
  }

  /**
   * @brief Waits for all work threads to join
   */
  ~work_group()
  {
    work_group::each([](auto& t) { t.join(); });
  }

private:
  /// Executes a callback on each worker thread
  template <typename UnaryFnT> void each(UnaryFnT&& fn)
  {
    std::for_each(workers_.begin(), workers_.end(), std::forward<UnaryFnT>(fn));
  }

  /// Worker threads
  std::array<std::thread, N> workers_;
};

/**
 * @brief Managers N threads of execution, decided at runtime, each of which run an identical work-loop
 *
 * Joins threads on destruction
 */
template <> class work_group<0>
{
public:
  /**
   * @brief Starts all workers running work callback \c f
   */
  template <typename WorkLoopFnT>
  explicit work_group(WorkLoopFnT f, const std::size_t n_workers = std::thread::hardware_concurrency()) :
      workers_{std::make_unique<std::thread[]>(n_workers)}, n_workers_{n_workers}
  {
    work_group::each([f](auto& t) { t = std::thread{f}; });
  }

  /**
   * @brief Waits for all work threads to join
   */
  ~work_group()
  {
    work_group::each([](auto& t) { t.join(); });
  }

private:
  /// Executes a callback on each worker thread
  template <typename UnaryFnT> void each(UnaryFnT&& fn)
  {
    std::for_each(workers_.get(), workers_.get() + n_workers_, std::forward<UnaryFnT>(fn));
  }

  /// Worker threads
  std::unique_ptr<std::thread[]> workers_;

  /// Worker thread count
  std::size_t n_workers_;
};

/**
 * @brief Managers 1 thread of execution which runs a work-loop
 *
 * Joins threads on destruction
 */
template <> class work_group<1>
{
public:
  /**
   * @brief Starts worker running work callback \c f
   */
  template <typename WorkLoopFnT> explicit work_group(WorkLoopFnT&& f) : worker_{std::forward<WorkLoopFnT>(f)} {}

  /**
   * @brief Waits for work thread to join
   */
  ~work_group() { worker_.join(); }

private:
  /// Worker thread
  std::thread worker_;
};

/**
 * @brief Represents a default, FIFO work queue
 */
template <typename WorkStorageT = std::function<void()>, typename WorkStorageAllocatorT = std::allocator<WorkStorageT>>
class work_queue_fifo_default
{
public:
  /**
   * @brief Returns next job to run
   * @warning behavior is undefined if <code>empty() == true</code>
   */
  WorkStorageT pop()
  {
    WorkStorageT next_job{std::move(c_.front())};
    c_.pop_front();
    return next_job;
  }

  /**
   * @brief Adds new \c work to the queue
   */
  template <typename WorkT> void enqueue(WorkT&& work) { c_.emplace_back(std::forward<WorkT>(work)); }

  /**
   * @brief Returns true if queue contains no work
   */
  constexpr bool empty() const { return c_.empty(); }

private:
  /// Underlying queue storage
  std::deque<WorkStorageT, WorkStorageAllocatorT> c_;
};

/**
 * @brief Represents a pool of 1 of workers (typically threads) which participate in executing enqueued work
 */
template <typename WorkGroupT, typename WorkQueueT, typename WorkPoolOptionsT> class worker_pool_base
{
public:
  /**
   * @brief Initializes workers
   *
   * @param work_group_args...  arguments, other than work loop callback, used to setup work group, i.e.
   * <code>WorkGroupT{loop_fn, work_group_args...}</code>
   */
  template <typename... WorkGroupArgTs>
  explicit worker_pool_base(WorkGroupArgTs&&... work_group_args) :
      working_{true},
      workers_{
        [this]() {
          std::unique_lock lock{work_queue_mutex_};
          // Keep doing work until stopped
          while (worker_pool_base::should_continue())
          {
            if (work_queue_.empty())
            {
              // If no work is available, wait for emplace
              work_queue_cv_.wait(lock);
            }
            else
            {
              // Get next work to do
              auto next_to_run = work_queue_.pop();

              // Unlock queue lock
              lock.unlock();

              // Do the work
              next_to_run();

              // Lock queue lock
              lock.lock();
            }
          }
        },
        std::forward<WorkGroupArgTs>(work_group_args)...}
  {}

  /**
   * @brief Enqueues new work
   */
  template <typename WorkT> void emplace(WorkT&& job)
  {
    // Adds work under lock
    {
      std::lock_guard lock{work_queue_mutex_};
      work_queue_.enqueue(std::forward<WorkT>(job));
    }
    // Signal that work is available
    work_queue_cv_.notify_one();
  }

  ~worker_pool_base()
  {
    // Stop work loop under look
    {
      std::lock_guard lock{work_queue_mutex_};
      working_ = false;
    }
    // Unblock any active waits
    work_queue_cv_.notify_all();
  }

private:
  /**
   * @brief Checks if worker loop should continue or break
   */
  bool should_continue() const
  {
    if constexpr (WorkPoolOptionsT::finish_all_work)
    {
      return working_ or !work_queue_.empty();
    }
    else
    {
      return working_;
    }
  }

  /// Protects concurrent access of work_queue_cv_
  std::mutex work_queue_mutex_;

  /// Condition variable to signal new work
  std::condition_variable work_queue_cv_;

  /// Indicates the work is still running
  bool working_;

  /// Queue of jobs to run
  WorkQueueT work_queue_;

  /// Workers which run jobs
  WorkGroupT workers_;
};

/**
 * @brief Default work pool execution options
 */
struct worker_pool_options_default
{
  /// Do not finish all work before shutdown
  static constexpr bool finish_all_work = false;
};

/**
 * @brief A single-threaded work
 */
using worker = worker_pool_base<work_group<1>, work_queue_fifo_default<>, worker_pool_options_default>;

/**
 * @brief A multi-threaded worker; thread count decided at runtime
 */
using worker_pool = worker_pool_base<work_group<0>, work_queue_fifo_default<>, worker_pool_options_default>;

/**
 * @brief A multi-threaded worker; thread count decided at compile-time
 */
template <std::size_t N>
using static_worker_pool = worker_pool_base<work_group<N>, work_queue_fifo_default<>, worker_pool_options_default>;

/**
 * @brief Enumerates the type of work tracking strategies
 */
enum class post_strategy
{
  blocking,
  nonblocking,
};

/**
 * @brief Enqueues work to a work pool and returns a tracker for that work
 */
template <
  post_strategy Strategy,
  typename WorkGroupT,
  typename WorkQueueT,
  typename WorkPoolOptionsT,
  typename WorkT,
  typename ResultT = std::invoke_result_t<std::remove_reference_t<WorkT>>>
[[nodiscard]] auto post(worker_pool_base<WorkGroupT, WorkQueueT, WorkPoolOptionsT>& wp, WorkT&& work)
{
  auto create_work = [](auto&& promise_sp, auto&& work) {
    return [p = std::move(promise_sp), w = std::forward<WorkT>(work)]() mutable {
      if constexpr (std::is_same_v<ResultT, void>)
      {
        w();
        p->set_value();
      }
      else
      {
        p->set_value(w());
      }
    };
  };

  if constexpr (Strategy == post_strategy::blocking)
  {
    auto p = std::make_shared<std::promise<ResultT>>();
    auto f = p->get_future();
    wp.emplace(create_work(p, std::forward<WorkT>(work)));
    return f;
  }
  else
  {
    auto p = std::make_shared<non_blocking_promise<ResultT>>();
    auto f = p->get_future();
    wp.emplace(create_work(p, std::forward<WorkT>(work)));
    return f;
  }
}

/**
 * @brief Enqueues work and creates a non-blocking tracker
 */
template <typename WorkerPoolT, typename WorkT> [[nodiscard]] auto post_nonblocking(WorkerPoolT&& wp, WorkT&& work)
{
  return post<post_strategy::nonblocking>(std::forward<WorkerPoolT>(wp), std::forward<WorkT>(work));
}

/**
 * @brief Enqueues work and creates a blocking tracker
 */
template <typename WorkerPoolT, typename WorkT> [[nodiscard]] auto post_blocking(WorkerPoolT&& wp, WorkT&& work)
{
  return post<post_strategy::blocking>(std::forward<WorkerPoolT>(wp), std::forward<WorkT>(work));
}

}  // namespace tyl::async