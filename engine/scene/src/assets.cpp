// S++ Standard Library
#include <string>

// Tyl
#include <tyl/engine/assets.hpp>
#include <tyl/engine/ecs.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/file_stream.hpp>
#include <tyl/serialization/mem_stream.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/std/chrono.hpp>
#include <tyl/serialization/std/filesystem.hpp>

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
  std::string,
  AssetLocation<tyl::audio::device::Sound>,
  AssetLocation<tyl::graphics::device::Texture>
>;
// clang-format on

AssetLoadingStatus LoadTextures(Assets& assets, SharedState& shared);

AssetLoadingStatus LoadSoundData(Assets& assets, SharedState& shared);

AssetLoadingSummary LoadAssets(Assets& assets, SharedState& shared)
{
  return {.textures = LoadTextures(assets, shared), .sound_data = LoadSoundData(assets, shared)};
}

}  // namespace tyl::engine

namespace tyl::serialization
{

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, Clock::Time> : std::true_type
{};

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, engine::AssetLocationType> : std::true_type
{};

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, engine::AssetError> : std::true_type
{};

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, engine::AssetInfo> : std::true_type
{};

template <typename ArchiveT, typename AssetT> struct serialize<ArchiveT, engine::AssetLocation<AssetT>>
{
  void operator()(ArchiveT& ar, engine::AssetLocation<AssetT>& asset_location)
  {
    ar& named{"path", asset_location.path};
    ar& named{"type", asset_location.type};
  }
};

template <typename OArchiveT> void save_assets(OArchiveT& oar, const engine::Assets& assets)
{
  {
    engine::serializable_registry_t<const engine::AssetComponents> registry{assets.registry};
    oar << named{"registry", registry};
  }
}

template <typename IArchiveT> void load_assets(IArchiveT& iar, engine::Assets& assets)
{
  {
    engine::serializable_registry_t<engine::AssetComponents> registry{assets.registry};
    iar >> named{"registry", registry};
  }
}

void save<binary_oarchive<file_handle_ostream>, engine::Assets>::operator()(
  binary_oarchive<file_handle_ostream>& oar,
  const engine::Assets& assets) const
{
  save_assets(oar, assets);
}

void load<binary_iarchive<file_handle_istream>, engine::Assets>::operator()(
  binary_iarchive<file_handle_istream>& iar,
  engine::Assets& assets) const
{
  load_assets(iar, assets);
}

void save<binary_oarchive<mem_ostream>, engine::Assets>::operator()(
  binary_oarchive<mem_ostream>& oar,
  const engine::Assets& assets) const
{
  save_assets(oar, assets);
}

void load<binary_iarchive<mem_istream>, engine::Assets>::operator()(
  binary_iarchive<mem_istream>& iar,
  engine::Assets& assets) const
{
  load_assets(iar, assets);
}

}  // namespace tyl::serialization