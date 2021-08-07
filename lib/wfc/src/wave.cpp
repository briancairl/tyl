// C++ Standard Library
#include <cstring>
#include <iomanip>
#include <ostream>

// Tyl
#include <tyl/wfc/wave.hpp>

namespace tyl::wfc
{

Wave::Wave(const std::size_t rows, const std::size_t cols, const std::size_t elements) :
    rows_{rows},
    cols_{cols},
    domain_chunk_len_{get_domain_chunk_length(elements)},
    data_{std::make_unique<DomainChunkType[]>(chunks())}
{
  std::fill(data_.get(), data_.get() + chunks(), 0);
  for (std::size_t i = 0; i < rows_; ++i)
  {
    for (std::size_t j = 0; j < cols_; ++j)
    {
      auto domain = (*this)(i, j);
      for (ElementID e = 0; e < elements; ++e)
      {
        domain.set(e);
      }
    }
  }
}

std::ostream& operator<<(std::ostream& os, const Wave& wave)
{
  for (std::size_t i = 0; i < wave.rows(); ++i)
  {
    for (std::size_t j = 0; j < wave.cols(); ++j)
    {
      if (const auto domain = wave(i, j); domain.is_collapsed())
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
  return os;
}

}  // namespace tyl::wfc