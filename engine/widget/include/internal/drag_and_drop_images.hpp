/**
 * @copyright 2023-present Brian Cairl
 *
 * @file drag_and_drop_images.hpp
 */
#pragma once

// C++ Standard Library
#include <algorithm>
#include <filesystem>
#include <vector>

// Tyl
#include <tyl/async.hpp>
#include <tyl/crtp.hpp>
#include <tyl/ecs.hpp>
#include <tyl/engine/internal/widget.hpp>
#include <tyl/expected.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>

namespace tyl::engine
{

/**
 * @brief Handles image drag and drop handling
 */
class DragAndDropImages
{
  using Image = graphics::host::Image;
  using Texture = graphics::device::Texture;

public:
  struct NotReady
  {};

  template <typename ShouldAccept, typename HandleError>
  expected<std::vector<EntityID>, NotReady> update(
    Registry& registry,
    WidgetSharedState& shared,
    const WidgetResources& resources,
    ShouldAccept should_accept,
    HandleError handle_error)
  {
    if (!loading_images_.empty())
    {
      if (std::any_of(loading_images_.begin(), loading_images_.end(), [](auto& f) { return !f.valid(); }))
      {
        return make_unexpected(NotReady{});
      }

      std::vector<EntityID> entities;
      entities.reserve(loading_images_.size());
      for (auto& f : loading_images_)
      {
        if (auto loaded = f.get(); loaded.image.has_value())
        {
          const auto id = registry.create();
          registry.emplace<Texture>(id, loaded.image->texture());
          registry.emplace<std::filesystem::path>(id, std::move(loaded.image_path));
          entities.push_back(id);
        }
        else
        {
          handle_error(loaded.image.error());
        }
      }
      loading_images_.clear();
      return entities;
    }
    else if (should_accept())
    {
      for (const auto& path : resources.drop_payloads)
      {
        loading_images_.emplace_back(async::post(shared.thread_pool, [path] {
          return LoadingImage{.image = Image::load(path), .image_path = std::move(path)};
        }));
      }
    }
    return make_unexpected(NotReady{});
  }

  template <typename ShouldAccept>
  expected<std::vector<EntityID>, NotReady>
  update(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources, ShouldAccept&& should_accept)
  {
    return update(
      registry, shared, resources, std::forward<ShouldAccept>(should_accept), []([[maybe_unused]] const auto& e) {});
  }

private:
  struct LoadingImage
  {
    expected<Image, Image::ErrorCode> image;
    std::filesystem::path image_path;
  };
  std::vector<async::non_blocking_future<LoadingImage>> loading_images_;
};

}  // namespace tyl::engine
