/**
 * @copyright 2023-present Brian Cairl
 *
 * @file script_tilemap_creator.cpp
 */

// C++ Standard Library

// Tyl
#include <tyl/assert.hpp>
#include <tyl/engine/asset.hpp>
#include <tyl/engine/ecs.hpp>
#include <tyl/engine/internal/imgui.hpp>
#include <tyl/engine/scene.hpp>
#include <tyl/engine/script/tile_map_creator.hpp>
#include <tyl/engine/tile_set.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/file_stream.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>

// 1. Create new named tileset
// 2. Select texture
// 3. Select grids

namespace tyl::engine
{
namespace
{}  // namespace

using namespace tyl::serialization;

class TileMapCreator::Impl
{
public:
};

using namespace tyl::serialization;

TileMapCreator::~TileMapCreator() = default;

tyl::expected<TileMapCreator, ScriptCreationError> TileMapCreator::CreateImpl(const TileMapCreatorOptions& options)
{
  return TileMapCreator{options, std::make_unique<Impl>()};
}

TileMapCreator::TileMapCreator(const TileMapCreatorOptions& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

template <> void TileMapCreator::SaveImpl(ScriptOArchive<file_handle_ostream>& oar) const {}  //{ impl_->Save(oar); }

template <> void TileMapCreator::LoadImpl(ScriptIArchive<file_handle_istream>& iar) {}  // { impl_->Load(iar); }

ScriptStatus TileMapCreator::UpdateImpl(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
{

  return ScriptStatus::kOk;
}

}  // namespace tyl::engine

namespace tyl::serialization
{}  // tyl::serialization
