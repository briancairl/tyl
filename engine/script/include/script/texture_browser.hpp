/**
 * @copyright 2023-present Brian Cairl
 *
 * @file script_texture_browser.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/engine/script/script.hpp>

namespace tyl::engine
{
class TextureBrowser;

struct TextureBrowserOptions
{
  const char* name = "Texture Browser";
};

template <> struct ScriptOptions<TextureBrowser>
{
  using type = TextureBrowserOptions;
};

class TextureBrowser : public ScriptBase<TextureBrowser>
{
  friend class ScriptBase<TextureBrowser>;

public:
  TextureBrowser(TextureBrowser&&) = default;

  ~TextureBrowser();

private:
  static expected<TextureBrowser, ScriptCreationError> CreateImpl(const TextureBrowserOptions& options);

  template <typename StreamT> void SaveImpl(ScriptOArchive<StreamT>& oar) const;

  template <typename StreamT> void LoadImpl(ScriptIArchive<StreamT>& iar);

  ScriptStatus UpdateImpl(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources);

  TextureBrowserOptions options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  TextureBrowser(const TextureBrowserOptions& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
