// C++ Standard Library
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <ostream>

// Tyl
#include <tyl/wfc/wave.hpp>
#include <tyl/wfc/wave_probability.hpp>

namespace tyl::wfc
{

WaveProbability::WaveProbability(const Wave& wave, const float* const p_elements, const std::size_t n_elements) :
    rows{wave.rows()},
    cols{wave.cols()},
    count{std::make_unique<std::size_t[]>(wave.size())},
    p{std::make_unique<float[]>(n_elements)},
    p_sum{std::make_unique<float[]>(wave.size())},
    p_log_p{std::make_unique<float[]>(n_elements)},
    p_log_p_sum{std::make_unique<float[]>(wave.size())}
{
  // Initialize the sum of probabilities at each wave cell
  std::fill(count.get(), count.get() + wave.size(), n_elements);

  // Copy in element probabilities
  std::copy(p_elements, p_elements + n_elements, p.get());

  // Compute p*log(p) of element probabilities
  std::transform(p_elements, p_elements + n_elements, p_log_p.get(), [](const float p) { return p * std::log2(p); });

  // Initialize the sum of probabilities at each wave cell
  std::fill(p_sum.get(), p_sum.get() + wave.size(), 1.f);

  // Initialize the sum of probabilities at each wave cell
  // Initialize the total entropy of each wave cell
  float e = 0.f;
  for (ElementID i = 0; i < n_elements; ++i)
  {
    e -= p[i] * p_log_p[i];
  }
  std::fill(p_log_p_sum.get(), p_log_p_sum.get() + wave.size(), e);
}


std::ostream& operator<<(std::ostream& os, const WaveProbability& wave_p)
{
  for (std::size_t i = 0; i < wave_p.rows; ++i)
  {
    for (std::size_t j = 0; j < wave_p.cols; ++j)
    {
      os << std::left << std::setw(12) << wave_p.remaining(i, j);
    }
    os << '\n';
  }
  return os;
}

}  // namespace tyl::wfc