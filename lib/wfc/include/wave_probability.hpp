/**
 * @copyright 2021-present Brian Cairl
 *
 * @file wave.hpp
 */
#pragma once

// C++ Standard Library
#include <cmath>
#include <iosfwd>
#include <memory>

// Tyl
#include <tyl/common/bitops.hpp>
#include <tyl/wfc/typedefs.hpp>

namespace tyl::wfc
{

class Wave;

struct WaveProbability
{
  WaveProbability(const Wave& wave, const float* const p_elements, const std::size_t n_elements);

  void collapse(const std::size_t i, const std::size_t j, const ElementID element_id)
  {
    const std::size_t l = i * cols + j;
    count[l] = 1;
    p_sum[l] = p[element_id];
    p_log_p_sum[l] = p_log_p[element_id];
  }

  bool is_collapsed(const std::size_t i, const std::size_t j) const
  {
    const std::size_t l = i * cols + j;
    return count[l] < 2;
  }

  void clear(const std::size_t i, const std::size_t j, const ElementID element_id)
  {
    const std::size_t l = i * cols + j;
    count[l]--;
    p_sum[l] -= p[element_id];
    p_log_p_sum[l] -= p_log_p[element_id];
  }

  float entropy(const std::size_t i, const std::size_t j) const
  {
    const std::size_t l = i * cols + j;
    const float normalizer = p_sum[l];
    return p_log_p_sum[l] - (normalizer * std::log2(normalizer));
  }

  float normalizer(const std::size_t i, const std::size_t j) const
  {
    const std::size_t l = i * cols + j;
    return p_sum[l];
  }

  std::size_t remaining(const std::size_t i, const std::size_t j) const
  {
    const std::size_t l = i * cols + j;
    return count[l];
  }

  std::size_t rows;
  std::size_t cols;
  std::unique_ptr<std::size_t[]> count;
  std::unique_ptr<float[]> p;
  std::unique_ptr<float[]> p_sum;
  std::unique_ptr<float[]> p_log_p;
  std::unique_ptr<float[]> p_log_p_sum;
};

std::ostream& operator<<(std::ostream& os, const WaveProbability& wave_p);

}  // namespace tyl::wfc
