/**
 * @copyright 2023-present Brian Cairl
 *
 * @file load_textures.hpp
 */

// Tyl
#include <tyl/audio/device/sound.hpp>
#include <tyl/audio/host/sound_data.hpp>
#include <tyl/engine/asset/load_type.hpp>
#include <tyl/engine/asset/loading.hpp>
#include <tyl/engine/asset/types.hpp>
#include <tyl/engine/common/resources.hpp>
#include <tyl/engine/ecs/types.hpp>
#include <tyl/expected.hpp>

namespace tyl::engine::asset
{

void LoadTextures(LoadStatus& status, Collection& collection, Resources& resources)
{
  LoadType<Sound, SoundData>(
    status,
    collection.registry,
    resources,
    [](const std::filesystem::path& path) -> expected<SoundData, Error> {
      if (path.extension() != ".wav")
      {
        return make_unexpected(Error::kInvalidPath);
      }

      if (auto sound_or_error = SoundData::load(path.c_str()); sound_or_error.has_value())
      {
        return std::move(sound_or_error).value();
      }
      return make_unexpected(Error::kFailedToLoad);
    },
    [](Registry& registry, EntityID id, SoundData&& sound_data) { registry.emplace<Sound>(id, sound_data.sound()); });
}

}  // namespace tyl::engine::asset