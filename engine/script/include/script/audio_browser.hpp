/**
 * @copyright 2023-present Brian Cairl
 *
 * @file audio_browser.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/engine/script/script.hpp>

namespace tyl::engine
{
class AudioBrowser;

struct AudioBrowserOptions
{
  const char* name = "Audio Browser";
};

template <> struct ScriptOptions<AudioBrowser>
{
  using type = AudioBrowserOptions;
};

class AudioBrowser : public ScriptBase<AudioBrowser>
{
  friend class ScriptBase<AudioBrowser>;

public:
  AudioBrowser(AudioBrowser&&) = default;

  ~AudioBrowser();

private:
  static expected<AudioBrowser, ScriptCreationError> CreateImpl(const AudioBrowserOptions& options);

  template <typename StreamT> void SaveImpl(ScriptOArchive<StreamT>& oar) const;

  template <typename StreamT> void LoadImpl(ScriptIArchive<StreamT>& iar);

  ScriptStatus UpdateImpl(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources);

  AudioBrowserOptions options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  AudioBrowser(const AudioBrowserOptions& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
