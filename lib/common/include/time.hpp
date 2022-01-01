/**
 * @copyright 2021-present Brian Cairl
 *
 * @file time.hpp
 */
#pragma once

// C++ Standard Library
#include <chrono>

namespace tyl
{

using clock = std::chrono::steady_clock;

using time_point = typename clock::time_point;

using duration = typename clock::duration;

using fseconds = std::chrono::duration<float>;

constexpr duration make_duration(float seconds)
{
  return std::chrono::duration_cast<duration>(std::chrono::duration<float>{seconds});
}

constexpr fseconds to_fseconds(const duration dur) { return std::chrono::duration_cast<fseconds>(dur); }

}  // namespace tyl
