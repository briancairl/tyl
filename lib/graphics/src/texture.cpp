/**
 * @copyright 2021-present Brian Cairl
 *
 * @file texture.cpp
 */

// Tyl
#include <tyl/filesystem.hpp>
#include <tyl/graphics/image.hpp>
#include <tyl/graphics/texture.hpp>

namespace tyl::graphics
{
namespace  // anonymous
{

Texture::ChannelMode image_channel_count_to_mode(const int count)
{
  switch (count)
  {
  case 1:
    return Texture::ChannelMode::R;
  case 2:
    return Texture::ChannelMode::RG;
  case 3:
    return Texture::ChannelMode::RGB;
  case 4:
    return Texture::ChannelMode::RGBA;
  default:
    break;
  }
  return Texture::ChannelMode::R;
}

Texture to_texture(const Image& image, const Texture::Options& options)
{
  return Texture{image.size(), image.data(), image_channel_count_to_mode(image.channels()), options};
}

}  // namespace anonymous

ecs::entity create_texture(ecs::registry& registry, const std::string_view filename, const Texture::Options& options)
{
  const ecs::entity e = registry.create();
  attach_texture(registry, e, filename, options);
  return e;
}

void attach_texture(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const std::string_view filename,
  const Texture::Options& options)
{
  registry.emplace<Texture>(entity_id, to_texture(Image::load_from_file(filename.data()), options));
  registry.emplace<tyl::filesystem::path>(entity_id, filename);
}

}  // namespace tyl::graphics
