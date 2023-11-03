/**
 * @copyright 2023-present Brian Cairl
 *
 * @file audio_browser.cpp
 */

// C++ Standard Library
#include <memory>
#include <optional>

// Tyl
#include <tyl/audio/device/device.hpp>
#include <tyl/audio/device/listener.hpp>
#include <tyl/audio/device/sound.hpp>
#include <tyl/audio/device/source.hpp>
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

struct AudioBrowserPreviewState
{
  bool is_selected = false;
};

}  // namespace

using namespace tyl::serialization;

class AudioBrowser::Impl
{
public:
  Impl() : audio_device_{}, audio_listener_{audio_device_} { audio_device_.enable(); }

  ~Impl() { audio_device_.disable(); }

  void Update(Scene& scene, WidgetSharedState& shared, const WidgetResources& resources)
  {
    DragAndDropExternalSink(scene, shared, resources);
    AddAudioBrowserPreviewState(scene);
    ShowSoundListing(scene);
  }

  void AddAudioBrowserPreviewState(Scene& scene)
  {

    // Add view state to all available texture assets
    scene.assets.view<AssetLocation<Sound>, Sound>(entt::exclude<AudioBrowserPreviewState>)
      .each([&scene](const EntityID id, const auto& sound, const auto& asset_location) {
        scene.assets.emplace<AudioBrowserPreviewState>(id);
      });

    if (ImGui::Button("delete"))
    {
      scene.assets.view<AudioBrowserPreviewState>().each([&](const EntityID id, const auto& state) {
        if (state.is_selected)
        {
          scene.assets.destroy(id);
        }
      });
    }
  }

  void ShowSoundListing(Scene& scene)
  {
    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_None;
    if (ImGui::BeginChild("#SoundPreviewsChild", ImVec2{0, 0}, kChildShowBoarders, kChildFlags))
    {
      if (ImGui::BeginTable("##SoundListing", 4, ImGuiTableFlags_Resizable))
      {
        ImGui::TableSetupColumn("##");
        ImGui::TableSetupColumn("path");
        ImGui::TableSetupColumn("size");
        ImGui::TableSetupColumn("id");
        ImGui::TableHeadersRow();

        scene.assets.view<AssetLocation<Sound>, AssetInfo, AudioBrowserPreviewState>().each(
          [&, drawlist = ImGui::GetWindowDrawList()](
            const EntityID id, const auto& asset_location, const auto& asset_info, auto& state) {
            const bool is_valid = (asset_info.error == AssetError::kNone);
            bool is_selected = state.is_selected;

            ImGui::TableNextColumn();
            {
              ImGui::PushID(static_cast<int>(id) + 1);
              ImGui::Checkbox("##", &is_selected);
              ImGui::PopID();
            }
            ImGui::TableNextColumn();
            {
              ImGui::Text("%s", asset_location.path.filename().string().c_str());
              if (is_valid && ImGui::IsItemClicked(ImGuiMouseButton_Left))
              {
                active_playback_.emplace(audio_source_.play(scene.assets.get<Sound>(id)));
              }
              DragAndDropInternalSource(scene, id, asset_location.path, state);
            }
            ImGui::TableNextColumn();
            {
              if (is_valid)
              {
                ImGui::Text("%lu kb", asset_info.size_in_bytes / 1000);
              }
              else
              {
                ImGui::Text("[n/a]");
              }
            }
            ImGui::TableNextColumn();
            {
              ImGui::Text("%d", static_cast<int>(id));
            }

            if (!is_valid)
            {
              ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImColor{1.f, 0.f, 0.f, 0.25f});
            }
            else if (is_selected)
            {
              ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImColor{1.f, 1.f, 0.f, 0.25f});
            }
            state.is_selected = is_selected;
          });

        ImGui::EndTable();
      }
      ImGui::EndChild();
    }
  }

  void DragAndDropInternalSource(
    const Scene& scene,
    const EntityID id,
    const std::filesystem::path& path,
    const AudioBrowserPreviewState& state) const
  {
    if (!ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
    {
      return;
    }

    ImVec4 tint{1, 1, 1, 1};
    if (ImGui::SetDragDropPayload("TYL_TEXTURE_ASSET", std::addressof(id), sizeof(EntityID), /*cond = */ 0))
    {
      tint = ImVec4{0, 1, 0, 1};
    }
    ImGui::TextColored(tint, "%s", path.filename().string().c_str());

    ImGui::EndDragDropSource();
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
  audio::device::Device audio_device_;
  audio::device::Listener audio_listener_;
  audio::device::Source audio_source_;
  std::optional<audio::device::Playback> active_playback_;
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