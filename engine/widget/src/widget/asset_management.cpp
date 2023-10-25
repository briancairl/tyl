/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget_texture_browser.cpp
 */

// C++ Standard Library
#include <algorithm>
#include <memory>
#include <numeric>

// Tyl
#include <tyl/engine/internal/imgui.hpp>
#include <tyl/engine/widget/asset_management.hpp>
#include <tyl/serialization/file_stream.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/packet.hpp>
#include <tyl/serialization/std/vector.hpp>

namespace tyl::serialization
{

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, Clock::Time> : std::true_type
{};

}  // tyl::serialization

namespace tyl::engine
{
using namespace tyl::serialization;

AssetManagement::~AssetManagement() = default;

tyl::expected<AssetManagement, WidgetCreationError> AssetManagement::CreateImpl(const AssetManagementOptions& options)
{
  return AssetManagement{options};
}

AssetManagement::AssetManagement(const AssetManagementOptions& options) : options_{options} {}

template <> void AssetManagement::SaveImpl(WidgetOArchive<file_handle_ostream>& oar, const Registry& registry) {}

template <> void AssetManagement::LoadImpl(WidgetIArchive<file_istream>& iar, Registry& registry) {}

WidgetStatus
AssetManagement::UpdateImpl(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources)
{


  static constexpr auto kStaticWindowFlags = ImGuiWindowFlags_None;
  if (ImGui::Begin(options_.load_bar_popup_title, nullptr, kStaticWindowFlags))
  {}
  ImGui::End();
  return WidgetStatus::kOk;
}

}  // namespace tyl::engine