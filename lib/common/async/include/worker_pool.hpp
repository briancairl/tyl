/**
 * @copyright 2023-present Brian Cairl
 *
 * @file thread_pool.hpp
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

namespace tyl::async
{

template <std::size_t N> class work_group
{
public:
  template <typename Fn> explicit work_group(Fn f)
  {
    for (auto& t : workers_)
    {
      t = std::thread{f};
    }
  }

  ~work_group()
  {
    for (auto& t : workers_)
    {
      t.join();
    }
  }

private:
  std::array<std::thread, N> workers_;
};

template <> class work_group<0>
{
public:
  template <typename Fn>
  work_group(Fn f, const std::size_t n_workers = std::thread::hardware_concurrency()) :
      workers_{std::make_unique<std::thread[]>(n_workers)}, n_workers_{n_workers}
  {
    std::for_each(workers_.get(), workers_.get() + n_workers_, [f](auto& t) { t = std::thread{f}; });
  }

  ~work_group()
  {
    std::for_each(workers_.get(), workers_.get() + n_workers_, [](auto& t) { t.join(); });
  }

private:
  std::unique_ptr<std::thread[]> workers_;
  std::size_t n_workers_;
};

template <> class work_group<1>
{
public:
  template <typename Fn> explicit work_group(Fn&& f) : worker_{std::forward<Fn>(f)} {}

  ~work_group() { worker_.join(); }

private:
  std::thread worker_;
};

template <typename WorkStorageT = std::function<void()>, typename WorkStorageAllocatorT = std::allocator<WorkStorageT>>
class work_queue
{
public:
  WorkStorageT pop()
  {
    WorkStorageT next_job{std::move(c_.front())};
    c_.pop_front();
    return next_job;
  }

  template <typename WorkT> void enqueue(WorkT&& work) { c_.emplace_back(std::forward<WorkT>(work)); }

  constexpr bool empty() const { return c_.empty(); }

private:
  std::deque<WorkStorageT, WorkStorageAllocatorT> c_;
};

template <typename WorkGroupT, typename WorkQueueT, bool kFinishAllWork = false> class worker_pool_base
{
public:
  template <typename... WorkGroupArgTs>
  worker_pool_base(WorkGroupArgTs&&... args) :
      working_{true},
      workers_{
        [this]() {
          std::unique_lock lock{work_queue_mutex_};
          while (working_ || (kFinishAllWork and !work_queue_.empty()))
          {
            work_queue_cv_.wait(lock);

            if (work_queue_.empty())
            {
              continue;
            }

            auto next_to_run = work_queue_.pop();

            lock.unlock();
            next_to_run();
            lock.lock();
          }
        },
        std::forward<WorkGroupArgTs>(args)...}
  {}

  template <typename WorkT> void emplace(WorkT&& job)
  {
    {
      std::lock_guard lock{work_queue_mutex_};
      work_queue_.enqueue(std::forward<WorkT>(job));
    }
    work_queue_cv_.notify_one();
  }

  ~worker_pool_base()
  {
    {
      std::lock_guard lock{work_queue_mutex_};
      working_ = false;
    }
    work_queue_cv_.notify_all();
  }

private:
  std::mutex work_queue_mutex_;
  std::condition_variable work_queue_cv_;
  bool working_;
  WorkQueueT work_queue_;
  WorkGroupT workers_;
};

using worker = worker_pool_base<work_group<1>, work_queue<>>;

using worker_pool = worker_pool_base<work_group<0>, work_queue<>>;

template <std::size_t N> using static_worker_pool = worker_pool_base<work_group<N>, work_queue<>>;

template <
  typename WorkGroupT,
  typename WorkQueueT,
  typename WorkT,
  typename ResultT = std::invoke_result_t<std::remove_reference_t<WorkT>>>
auto post(worker_pool_base<WorkGroupT, WorkQueueT>& wp, WorkT&& work) -> std::future<ResultT>
{
  auto p = std::make_shared<std::promise<ResultT>>();
  auto f = p->get_future();
  wp.emplace([p = std::move(p), w = std::forward<WorkT>(work)]() mutable { p->set_value(w()); });
  return f;
}

}  // namespace tyl::async