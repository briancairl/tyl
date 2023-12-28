/**
 * @copyright 2023-present Brian Cairl
 *
 * @file load_textures.hpp
 */
// C++ Standard Library
#include <algorithm>
#include <memory>
#include <numeric>

// Tyl
#include <tyl/audio/device/sound.hpp>
#include <tyl/audio/host/sound_data.hpp>
#include <tyl/ecs.hpp>
#include <tyl/engine/assets/load.hpp>
#include <tyl/engine/shared_state.hpp>

namespace tyl::engine
{

using SoundData = audio::host::SoundData;
using Sound = audio::device::Sound;

AssetLoadingStatus LoadTextures(Assets& assets, SharedState& shared_state)
{
  return Load<Sound, SoundData>(
    assets.registry,
    shared_state,
    [](const std::filesystem::path& path) -> expected<SoundData, AssetError> {
      if (path.extension() != ".wav")
      {
        return make_unexpected(AssetError::kInvalidPath);
      }

      if (auto sound_or_error = SoundData::load(path.c_str()); sound_or_error.has_value())
      {
        return std::move(sound_or_error).value();
      }
      return make_unexpected(AssetError::kFailedToLoad);
    },
    [](Registry& registry, EntityID id, SoundData&& sound_data) { registry.emplace<Sound>(id, sound_data.sound()); });
}

}  // namespace tyl::engine