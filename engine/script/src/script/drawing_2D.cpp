/**
 * @copyright 2023-present Brian Cairl
 *
 * @file drawing_2D.cpp
 */

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/engine/asset.hpp>
#include <tyl/engine/internal/imgui.hpp>
#include <tyl/engine/scene.hpp>
#include <tyl/engine/script/drawing_2D.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/file_stream.hpp>
#include <tyl/serialization/named.hpp>

#include <tyl/rect.hpp>

namespace tyl::engine
{
namespace
{

struct Drawing2DProperties
{};

}  // namespace

using namespace tyl::serialization;

class Drawing2D::Impl
{
public:
  Impl() {}

  void Update(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources) {}

  template <typename OArchive> void Save(OArchive& ar) const { ar << named{"properties", properties_}; }

  template <typename IArchive> void Load(IArchive& ar) { ar >> named{"properties", properties_}; }

private:
  Drawing2DProperties properties_;
};

Drawing2D::~Drawing2D() = default;

template <> void Drawing2D::SaveImpl(ScriptOArchive<file_handle_ostream>& oar) const { impl_->Save(oar); }

template <> void Drawing2D::LoadImpl(ScriptIArchive<file_handle_istream>& iar) { impl_->Load(iar); }

tyl::expected<Drawing2D, ScriptCreationError> Drawing2D::CreateImpl(const Drawing2DOptions& options)
{
  return Drawing2D{options, std::make_unique<Impl>()};
}

Drawing2D::Drawing2D(const Drawing2DOptions& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

ScriptStatus Drawing2D::UpdateImpl(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
{
  static constexpr auto kStaticWindowFlags = ImGuiWindowFlags_HorizontalScrollbar;
  if (ImGui::Begin(options_.name, nullptr, kStaticWindowFlags))
  {
    if (scene.active_camera.has_value())
    {
      impl_->Update(scene, shared, resources);
    }
    else
    {
      ImGui::TextColored(ImVec4{1, 0, 0, 1}, "no active camera");
    }
  }
  ImGui::End();
  return ScriptStatus::kOk;
}

}  // namespace tyl::engine

namespace tyl::serialization
{

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, engine::Drawing2DProperties> : std::true_type
{};

}  // tyl::serialization