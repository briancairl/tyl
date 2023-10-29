/**
 * @copyright 2023-present Brian Cairl
 *
 * @file audio_browser.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/engine/widget/widget.hpp>

namespace tyl::engine
{
class AudioBrowser;

struct AudioBrowserOptions
{
  const char* name = "Audio Browser";
};

template <> struct WidgetOptions<AudioBrowser>
{
  using type = AudioBrowserOptions;
};

class AudioBrowser : public WidgetBase<AudioBrowser>
{
  friend class WidgetBase<AudioBrowser>;

public:
  AudioBrowser(AudioBrowser&&) = default;

  ~AudioBrowser();

private:
  static expected<AudioBrowser, WidgetCreationError> CreateImpl(const AudioBrowserOptions& options);

  template <typename StreamT> void SaveImpl(WidgetOArchive<StreamT>& oar) const;

  template <typename StreamT> void LoadImpl(WidgetIArchive<StreamT>& iar);

  WidgetStatus UpdateImpl(Scene& scene, WidgetSharedState& shared, const WidgetResources& resources);

  AudioBrowserOptions options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  AudioBrowser(const AudioBrowserOptions& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
