// S++ Standard Library
#include <string>

// Tyl
#include <tyl/engine/camera.hpp>
#include <tyl/engine/drawing.hpp>
#include <tyl/engine/ecs.hpp>
#include <tyl/engine/scene.hpp>
#include <tyl/engine/tile_map.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/file_stream.hpp>
#include <tyl/serialization/mem_stream.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/std/optional.hpp>

namespace tyl::engine
{

// clang-format off
using SceneComponents = Components<
  std::string,
  Rect2f,
  TileMap,
  TileMapSection,
  TopDownCamera2D
>;
// clang-format on

}  // namespace tyl::engine

namespace tyl::serialization
{

template <typename OArchiveT> void save_scene(OArchiveT& oar, const engine::Scene& scene)
{
  {
    engine::serializable_registry_t<const engine::SceneComponents> registry{scene.registry};
    oar << named{"registry", registry};
  }
  oar << named{"active_camera", scene.active_camera};
}

template <typename IArchiveT> void load_scene(IArchiveT& iar, engine::Scene& scene)
{
  {
    engine::serializable_registry_t<engine::SceneComponents> registry{scene.registry};
    iar >> named{"registry", registry};
  }
  iar >> named{"active_camera", scene.active_camera};
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