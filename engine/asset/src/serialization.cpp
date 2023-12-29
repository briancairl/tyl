// S++ Standard Library
#include <string>

// Tyl
#include <tyl/engine/asset/serialization.hpp>
#include <tyl/engine/asset/types.hpp>
#include <tyl/engine/ecs/serialization.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/file_stream.hpp>
#include <tyl/serialization/mem_stream.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/std/chrono.hpp>
#include <tyl/serialization/std/filesystem.hpp>

namespace tyl::serialization
{
using namespace tyl::engine;
using namespace tyl::engine::asset;

// clang-format off
using Components = Components<
  Label,
  Location<Sound>,
  Location<Texture>
>;
// clang-format on

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, Clock::Time> : std::true_type
{};

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, LocationType> : std::true_type
{};

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, Error> : std::true_type
{};

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, Info> : std::true_type
{};

template <typename ArchiveT, typename AssetT> struct serialize<ArchiveT, Location<AssetT>>
{
  void operator()(ArchiveT& ar, Location<AssetT>& asset_location)
  {
    ar& named{"path", asset_location.path};
    ar& named{"type", asset_location.type};
  }
};

template <typename OArchiveT> void save_collection(OArchiveT& oar, const Collection& collection)
{
  {
    engine::serializable_registry_t<const Components> registry{collection.registry};
    oar << named{"registry", registry};
  }
}

template <typename IArchiveT> void load_collection(IArchiveT& iar, Collection& collection)
{
  {
    engine::serializable_registry_t<Components> registry{collection.registry};
    iar >> named{"registry", registry};
  }
}

void save<binary_oarchive<file_handle_ostream>, Collection>::operator()(
  binary_oarchive<file_handle_ostream>& oar,
  const Collection& collection) const
{
  save_collection(oar, collection);
}

void load<binary_iarchive<file_handle_istream>, Collection>::operator()(
  binary_iarchive<file_handle_istream>& iar,
  Collection& collection) const
{
  load_collection(iar, collection);
}

void save<binary_oarchive<mem_ostream>, Collection>::operator()(
  binary_oarchive<mem_ostream>& oar,
  const Collection& collection) const
{
  save_collection(oar, collection);
}

void load<binary_iarchive<mem_istream>, Collection>::operator()(
  binary_iarchive<mem_istream>& iar,
  Collection& collection) const
{
  load_collection(iar, collection);
}

}  // namespace tyl::serialization