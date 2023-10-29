/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget_texture_browser.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/engine/widget/base.hpp>

namespace tyl::engine
{
class TextureBrowser;

struct TextureBrowserOptions
{
  const char* name = "Texture Browser";
};

template <> struct WidgetOptions<TextureBrowser>
{
  using type = TextureBrowserOptions;
};

class TextureBrowser : public WidgetBase<TextureBrowser>
{
  friend class WidgetBase<TextureBrowser>;

public:
  TextureBrowser(TextureBrowser&&) = default;

  ~TextureBrowser();

private:
  static expected<TextureBrowser, WidgetCreationError> CreateImpl(const TextureBrowserOptions& options);

  template <typename StreamT> void SaveImpl(WidgetOArchive<StreamT>& oar) const;

  template <typename StreamT> void LoadImpl(WidgetIArchive<StreamT>& iar);

  WidgetStatus UpdateImpl(Scene& scene, WidgetSharedState& shared, const WidgetResources& resources);

  TextureBrowserOptions options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  TextureBrowser(const TextureBrowserOptions& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
