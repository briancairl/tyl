// Tyl
#include <tyl/engine/asset.hpp>
#include <tyl/engine/ecs.hpp>
#include <tyl/engine/scene.hpp>
#include <tyl/engine/tile_map.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/file_stream.hpp>
#include <tyl/serialization/mem_stream.hpp>
#include <tyl/serialization/named.hpp>

namespace tyl::graphics::device
{
class Texture;
}  // tyl::graphics::device

namespace tyl::audio::device
{
class Sound;
}  // tyl::audio::device

namespace tyl::engine
{

// clang-format off
using AssetComponents = Components<
  AssetLocation<tyl::audio::device::Sound>,
  AssetLocation<tyl::graphics::device::Texture>
>;
using Graphics2DComponents = Components<
  TileMap,
  TileMapSection
>;
// clang-format on

}  // namespace tyl::engine

namespace tyl::serialization
{

template <typename OArchiveT> void save_scene(OArchiveT& oar, const engine::Scene& scene)
{
  {
    engine::serializable_registry_t<const engine::AssetComponents> assets{scene.assets};
    oar << named{"assets", assets};
  }
  {
    engine::serializable_registry_t<const engine::Graphics2DComponents> graphics_2D{scene.graphics_2D};
    oar << named{"graphics_2D", graphics_2D};
  }
}

template <typename IArchiveT> void load_scene(IArchiveT& iar, engine::Scene& scene)
{
  {
    engine::serializable_registry_t<engine::AssetComponents> assets{scene.assets};
    iar >> named{"assets", assets};
  }
  {
    engine::serializable_registry_t<engine::Graphics2DComponents> graphics_2D{scene.graphics_2D};
    iar >> named{"graphics_2D", graphics_2D};
  }
}

void save<binary_oarchive<file_handle_ostream>, engine::Scene>::operator()(
  binary_oarchive<file_handle_ostream>& oar,
  const engine::Scene& scene) const
{
  save_scene(oar, scene);
}

void load<binary_iarchive<file_handle_istream>, engine::Scene>::operator()(
  binary_iarchive<file_handle_istream>& iar,
  engine::Scene& scene) const
{
  load_scene(iar, scene);
}

void save<binary_oarchive<mem_ostream>, engine::Scene>::operator()(
  binary_oarchive<mem_ostream>& oar,
  const engine::Scene& scene) const
{
  save_scene(oar, scene);
}

void load<binary_iarchive<mem_istream>, engine::Scene>::operator()(
  binary_iarchive<mem_istream>& iar,
  engine::Scene& scene) const
{
  load_scene(iar, scene);
}

}  // namespace tyl::serialization