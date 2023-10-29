/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget_texture_browser.cpp
 */

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/audio/device/sound.hpp>
#include <tyl/engine/asset.hpp>
#include <tyl/engine/internal/imgui.hpp>
#include <tyl/engine/scene.hpp>
#include <tyl/engine/widget/audio_browser.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/file_stream.hpp>
#include <tyl/serialization/named.hpp>

#include <tyl/rect.hpp>

namespace tyl::engine
{
namespace
{
using Sound = audio::device::Sound;

struct AudioBrowserProperties
{
  float playback_volume = 0.1f;
};

}  // namespace

using namespace tyl::serialization;

class AudioBrowser::Impl
{
public:
  Impl() {}

  void Update(Scene& scene, WidgetSharedState& shared, const WidgetResources& resources)
  {
    DragAndDropExternalSink(scene, shared, resources);
  }

  void DragAndDropExternalSink(Scene& scene, WidgetSharedState& shared, const WidgetResources& resources)
  {
    for (const auto& path : resources.drop_payloads)
    {
      const auto id = scene.assets.create();
      scene.assets.emplace<AssetLocation<Sound>>(id, path);
    }
  }

  constexpr bool LockWindowMovement() const { return lock_window_movement_; }

  template <typename OArchive> void Save(OArchive& ar) const { ar << named{"properties", properties_}; }

  template <typename IArchive> void Load(IArchive& ar) { ar >> named{"properties", properties_}; }

private:
  bool lock_window_movement_ = false;
  AudioBrowserProperties properties_;
};

AudioBrowser::~AudioBrowser() = default;

template <> void AudioBrowser::SaveImpl(WidgetOArchive<file_handle_ostream>& oar) const { impl_->Save(oar); }

template <> void AudioBrowser::LoadImpl(WidgetIArchive<file_handle_istream>& iar) { impl_->Load(iar); }

tyl::expected<AudioBrowser, WidgetCreationError> AudioBrowser::CreateImpl(const AudioBrowserOptions& options)
{
  return AudioBrowser{options, std::make_unique<Impl>()};
}

AudioBrowser::AudioBrowser(const AudioBrowserOptions& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

WidgetStatus AudioBrowser::UpdateImpl(Scene& scene, WidgetSharedState& shared, const WidgetResources& resources)
{
  static constexpr auto kStaticWindowFlags = ImGuiWindowFlags_HorizontalScrollbar;
  if (ImGui::Begin(
        options_.name, nullptr, (impl_->LockWindowMovement() ? ImGuiWindowFlags_NoMove : 0) | kStaticWindowFlags))
  {
    impl_->Update(scene, shared, resources);
  }
  ImGui::End();
  return WidgetStatus::kOk;
}

}  // namespace tyl::engine

namespace tyl::serialization
{

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, engine::AudioBrowserProperties> : std::true_type
{};

}  // tyl::serialization