// Tyl
#include <tyl/engine/asset.hpp>
#include <tyl/engine/ecs.hpp>
#include <tyl/engine/scene.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/file_stream.hpp>
#include <tyl/serialization/mem_stream.hpp>
#include <tyl/serialization/named.hpp>

namespace tyl::graphics::device
{
class Texture;
}  // tyl::graphics::device

namespace tyl::engine
{

// clang-format off
using AssetComponents = Components<
  AssetLocation<tyl::graphics::device::Texture>
>;
// clang-format on

}  // namespace tyl::engine

namespace tyl::serialization
{

void save<binary_oarchive<file_handle_ostream>, engine::Scene>::operator()(
  binary_oarchive<file_handle_ostream>& oar,
  const engine::Scene& scene) const
{
  engine::serializable_registry_t<const engine::AssetComponents> assets{scene.assets};
  oar << named{"assets", assets};
}

void load<binary_iarchive<file_handle_istream>, engine::Scene>::operator()(
  binary_iarchive<file_handle_istream>& iar,
  engine::Scene& scene) const
{
  engine::serializable_registry_t<engine::AssetComponents> assets{scene.assets};
  iar >> named{"assets", assets};
}

void save<binary_oarchive<mem_ostream>, engine::Scene>::operator()(
  binary_oarchive<mem_ostream>& oar,
  const engine::Scene& scene) const
{
  engine::serializable_registry_t<const engine::AssetComponents> assets{scene.assets};
  oar << named{"assets", assets};
}

void load<binary_iarchive<mem_istream>, engine::Scene>::operator()(
  binary_iarchive<mem_istream>& iar,
  engine::Scene& scene) const
{
  engine::serializable_registry_t<engine::AssetComponents> assets{scene.assets};
  iar >> named{"assets", assets};
}

}  // namespace tyl::serialization