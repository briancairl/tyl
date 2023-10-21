/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tileset_creator.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/engine/widget.hpp>

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

class TextureBrowser : public Widget<TextureBrowser>
{
  friend class Widget<TextureBrowser>;

public:
  TextureBrowser(TextureBrowser&&) = default;

  ~TextureBrowser();

private:
  static expected<TextureBrowser, WidgetCreationError> CreateImpl(const TextureBrowserOptions& options);

  WidgetStatus UpdateImpl(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources);

  TextureBrowserOptions options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  TextureBrowser(const TextureBrowserOptions& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
