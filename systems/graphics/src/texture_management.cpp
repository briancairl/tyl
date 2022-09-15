/**
 * @copyright 2022-present Brian Cairl
 *
 * @file texture_management.cpp
 */

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>
#include <tyl/graphics/texture_management.hpp>
#include <tyl/math/vec.hpp>

namespace tyl::graphics
{
namespace  // anonymous
{

TextureReference load_texture(ecs::registry& reg, const ecs::entity e, const TextureSourceImageAssetData& asset_data)
{
  const auto texture_host = host::load(
    asset_data.path.c_str(),
    {.channel_mode = host::ImageOptions::ChannelMode::Default,
     .flags = {.flip_vertically = asset_data.flip_vertically}});

  reg.emplace<TextureSize>(e, texture_host.height(), texture_host.width());

  const auto& texture = reg.emplace<device::Texture>(e, texture_host);

  return {e, texture};
}

}  // namespace anonymous

TextureReference attach_texture(ecs::registry& reg, const ecs::entity e, const TextureSourceImageAssetData& asset_data)
{
  reg.emplace<TextureSourceImageAssetData>(e, asset_data);
  return load_texture(reg, e, asset_data);
}

TextureReference create_texture(ecs::registry& reg, const TextureSourceImageAssetData& asset_data)
{
  const auto e = reg.create();
  return attach_texture(reg, e, asset_data);
}

void reload_textures(ecs::registry& reg)
{
  // Load all textures with associated asset data, but haven't yet been uploaded to the device
  auto view = reg.view<TextureSourceImageAssetData>(entt::exclude<device::Texture>);
  for (const auto e : view)
  {
    load_texture(reg, e, view.get<TextureSourceImageAssetData>(e));
  }
}

}  // namespace tyl::graphics