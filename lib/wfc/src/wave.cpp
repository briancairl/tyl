// C++ Standard Library
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <limits>
#include <ostream>

// Tyl
#include <tyl/common/assert.hpp>
#include <tyl/common/bitops.hpp>
#include <tyl/wfc/wave.hpp>

namespace tyl::wfc
{
namespace  // anonymous
{

static constexpr std::size_t get_domain_chunk_length(const std::size_t n_elements)
{
  return (n_elements / DomainChunkBits) + ((n_elements % DomainChunkBits) ? 1 : 0);
}

}  // anonymous

Wave::Wave(const WaveParameters& parameters) :
    layer_count_{parameters.layer_count},
    row_count_{parameters.row_count},
    col_count_{parameters.col_count},
    element_count_{parameters.element_probabilities.size()},
    domain_chunk_len_{get_domain_chunk_length(elements())},
    state_{std::make_unique<DomainChunkType[]>(chunks())},
    count_{std::make_unique<std::size_t[]>(size())},
    p_{std::make_unique<float[]>(elements())},
    p_sum_{std::make_unique<float[]>(size())},
    p_log_p_{std::make_unique<float[]>(elements())},
    p_log_p_sum_{std::make_unique<float[]>(size())}
{
  reset(parameters.element_probabilities);
}

void Wave::reset_cell_state()
{
  TYL_ASSERT_NON_NULL(state_);

  // Create a buffer to replicate
  std::unique_ptr<DomainChunkType[]> chunk_reset_buffer{std::make_unique<DomainChunkType[]>(domain_chunk_len_)};
  {
    const std::size_t chunks_whole = element_count_ / DomainChunkBits;

    TYL_ASSERT_LE(chunks_whole, domain_chunk_len_);

    std::fill(
      chunk_reset_buffer.get(), chunk_reset_buffer.get() + chunks_whole, std::numeric_limits<DomainChunkType>::max());
    if (const auto remainder = element_count_ % DomainChunkBits; remainder != 0)
    {
      chunk_reset_buffer.get()[chunks_whole] = bitops::make_mask_first_n<DomainChunkType>(remainder);
    }
  }

  // Fill every domain with the replication buffer values
  DomainChunkType* curr_chunk_ptr = state_.get();
  DomainChunkType* const last_chunk_ptr = state_.get() + chunks();
  while (curr_chunk_ptr != last_chunk_ptr)
  {
    // This should optimize to a memcpy
    curr_chunk_ptr = std::copy(chunk_reset_buffer.get(), chunk_reset_buffer.get() + domain_chunk_len_, curr_chunk_ptr);
  }
}

void Wave::reset_cell_probabilities(const ArrayView<const float>& element_probabilities)
{
  TYL_ASSERT_EQ(element_probabilities.size(), element_count_);
  TYL_ASSERT_NON_NULL(p_);
  TYL_ASSERT_NON_NULL(p_sum_);
  TYL_ASSERT_NON_NULL(p_log_p_);
  TYL_ASSERT_NON_NULL(p_log_p_sum_);

  // Initialize the sum of probabilities at each wave cell
  std::fill(count_.get(), count_.get() + size(), element_count_);

  // Copy in element probabilities
  std::copy(element_probabilities.begin(), element_probabilities.end(), p_.get());

  // Compute p*log(p) of element probabilities
  std::transform(element_probabilities.begin(), element_probabilities.end(), p_log_p_.get(), [](const float p) {
    return p * std::log2(p);
  });

  // Initialize the sum of probabilities at each wave cell
  std::fill(p_sum_.get(), p_sum_.get() + size(), 1.f);

  // Compute total entropy of an initial cell
  float negative_entropy = 0.f;
  for (ElementID i = 0; i < element_count_; ++i)
  {
    negative_entropy += p_log_p_[i];
  }

  // Initialize the total entropy of each wave cell
  std::fill(p_log_p_sum_.get(), p_log_p_sum_.get() + size(), negative_entropy);
}

void Wave::reset(const ArrayView<const float>& element_probabilities)
{
  reset_cell_state();
  reset_cell_probabilities(element_probabilities);
}

std::ostream& operator<<(std::ostream& os, const Wave& wave)
{
  for (std::size_t layer = 0; layer < wave.layer_count_; ++layer)
  {
    os << "layer: " << layer << '\n';
    for (std::size_t i = 0; i < wave.row_count_; ++i)
    {
      for (std::size_t j = 0; j < wave.col_count_; ++j)
      {
        if (const auto domain = wave(layer, i, j); domain.is_collapsed())
        {
          os << std::left << std::setw(2) << domain.id();
        }
        else
        {
          os << std::left << std::setw(2) << '~';
        }
      }
      os << '\n';
    }
    os << '\n';
  }
  return os;
}

}  // namespace tyl::wfc