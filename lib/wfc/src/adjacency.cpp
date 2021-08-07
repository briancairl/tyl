// C++ Standard Library
#include <cstring>

// Tyl
#include <tyl/wfc/adjacency.hpp>

namespace tyl::wfc
{

AdjacencyTable::AdjacencyTable(const std::size_t element_count) :
    element_count_{element_count},
    data_{std::make_unique<Adjacency[]>(element_count * element_count)}
{
  clear();
}

void AdjacencyTable::clear()
{
  static constexpr int ZERO_MEM = 0;
  std::memset(data_.get(), ZERO_MEM, sizeof(Adjacency) * size());
}

}  // namespace tyl::wfc