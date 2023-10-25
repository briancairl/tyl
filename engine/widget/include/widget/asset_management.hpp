/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget_tileset_creator.hpp
 */
#pragma once

// Tyl
#include <tyl/engine/widget/base.hpp>

namespace tyl::engine
{
class AssetManagement;

struct AssetManagementOptions
{
  const char* load_bar_popup_title = "Assets Loading...";
};

template <> struct WidgetOptions<AssetManagement>
{
  using type = AssetManagementOptions;
};

class AssetManagement : public WidgetBase<AssetManagement>
{
  friend class WidgetBase<AssetManagement>;

public:
  AssetManagement(AssetManagement&&) = default;

  ~AssetManagement();

private:
  static expected<AssetManagement, WidgetCreationError> CreateImpl(const AssetManagementOptions& options);

  template <typename StreamT> static void SaveImpl(WidgetOArchive<StreamT>& oar, const Registry& registry);

  template <typename StreamT> static void LoadImpl(WidgetIArchive<StreamT>& iar, Registry& registry);

  WidgetStatus UpdateImpl(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources);

  AssetManagementOptions options_;

  AssetManagement(const AssetManagementOptions& options);
};

}  // namespace tyl::engine
