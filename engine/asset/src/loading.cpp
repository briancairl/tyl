// Tyl
#include <tyl/engine/asset/loading.hpp>

namespace tyl::engine::asset
{

void LoadTextures(LoadStatus& status, Collection& collection, Resources& resources);

void LoadSoundData(LoadStatus& status, Collection& collection, Resources& resources);

LoadStatus Load(Collection& collection, Resources& resources)
{
  LoadStatus status;
  LoadTextures(status, collection, resources);
  LoadSoundData(status, collection, resources);
  return status;
}

}  // namespace tyl::engine::asset
