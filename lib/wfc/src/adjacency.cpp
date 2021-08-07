// C++ Standard Library
#include <cstring>

// Tyl
#include <tyl/wfc/adjacency.hpp>

namespace tyl::wfc::adjacency
{

Table::Table(const std::size_t element_count) :
    element_count_{element_count},
    data_{std::make_unique<StorageType[]>(element_count * element_count)}
{
  reset();
}

void Table::reset()
{
  static constexpr int ZERO_MEM = 0;
  std::memset(data_.get(), ZERO_MEM, sizeof(StorageType) * size());
}

}  // namespace tyl::wfc::adjacency