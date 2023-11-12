/**
 * @copyright 2023-present Brian Cairl
 *
 * @file asset_management.hpp
 */
#pragma once

// Tyl
#include <tyl/engine/script/script.hpp>

namespace tyl::engine
{
class AssetManagement;

struct AssetManagementOptions
{
  const char* name = "Asset Status";
};

template <> struct ScriptOptions<AssetManagement>
{
  using type = AssetManagementOptions;
};

class AssetManagement : public ScriptBase<AssetManagement>
{
  friend class ScriptBase<AssetManagement>;

public:
  AssetManagement(AssetManagement&&) = default;

  ~AssetManagement();

private:
  static expected<AssetManagement, ScriptCreationError> CreateImpl(const AssetManagementOptions& options);

  template <typename StreamT> static void SaveImpl(ScriptOArchive<StreamT>& oar);

  template <typename StreamT> static void LoadImpl(ScriptIArchive<StreamT>& iar);

  ScriptStatus UpdateImpl(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources);

  AssetManagementOptions options_;

  AssetManagement(const AssetManagementOptions& options);
};

}  // namespace tyl::engine
