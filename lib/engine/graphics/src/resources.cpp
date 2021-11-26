/**
 * @copyright 2021-present Brian Cairl
 *
 * @file resources.cpp
 */

// Tyl
#include <tyl/graphics/components.hpp>
#include <tyl/graphics/image.hpp>
#include <tyl/graphics/resources.hpp>
#include <tyl/graphics/shader.hpp>
#include <tyl/graphics/texture.hpp>

namespace tyl::graphics
{

namespace  // anonymous
{

inline filesystem::path replace_extension(filesystem::path original, const filesystem::path& ext)
{
  original.replace_extension(ext);
  return original;
}

}  // namespace anonymous

ecs::Entity load_shader(ecs::Registry& registry, const filesystem::path& path)
{
  const auto entity = registry.create();

  const filesystem::path vert_path{replace_extension(path, ".vert")};
  const filesystem::path frag_path{replace_extension(path, ".frag")};

  registry.emplace<filesystem::path>(entity, path);
  registry.emplace<Shader>(
    entity,
    ShaderSource::load_from_file(vert_path.c_str(), ShaderType::VERTEX),
    ShaderSource::load_from_file(frag_path.c_str(), ShaderType::FRAGMENT));
  return entity;
}

ecs::Entity load_texture(ecs::Registry& registry, const filesystem::path& path)
{
  const auto image = Image::load_from_file(path.c_str());
  const auto entity = registry.create();
  registry.emplace<filesystem::path>(entity, path);
  registry.emplace<TotalSizePx>(entity, image.rows(), image.cols());
  registry.emplace<Texture>(entity, image);
  return entity;
}

}  // namespace tyl::graphics
