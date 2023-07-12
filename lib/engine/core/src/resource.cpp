/**
 * @copyright 2023-present Brian Cairl
 *
 * @file primitives_renderer.cpp
 */

// C++ Standard Library
#include <ostream>
#include <unordered_map>
#include <unordered_set>

// Tyl
#include <tyl/core/engine/resource.hpp>

namespace tyl::engine::core::resource
{
namespace
{

struct HashPath
{
  std::size_t operator()(const Path& p) const { return std::filesystem::hash_value(p); }
};

std::optional<TypeCode> resolve_type(const Path& path)
{
  static const std::unordered_set<Path, HashPath> TEXTURE_EXTENSIONS{".png", ".jpg"};
  static const std::unordered_set<Path, HashPath> AUDIO_EXTENSIONS{".wav", ".mp3"};
  static const std::unordered_set<Path, HashPath> TEXT_EXTENSIONS{".txt", ".glsl"};

  const auto ext = path.extension();
  if (TEXTURE_EXTENSIONS.count(ext))
  {
    return TypeCode::TEXTURE;
  }
  else if (AUDIO_EXTENSIONS.count(ext))
  {
    return TypeCode::AUDIO;
  }
  else if (TEXT_EXTENSIONS.count(ext))
  {
    return TypeCode::TEXT;
  }
  return std::nullopt;
}

using LoadedResourceMap = std::unordered_map<Path, entt::entity, HashPath>;

LoadedResourceMap& get_loaded_resource_map(entt::registry& reg)
{
  auto& ctx = reg.ctx();
  if (auto* const rm = ctx.find<LoadedResourceMap>(); rm == nullptr)
  {
    return ctx.emplace<LoadedResourceMap>();
  }
  else
  {
    return *rm;
  }
}

template <typename ResourceT>
std::optional<ErrorCode> load(entt::registry& reg, const entt::entity id, const Path& path)
{
  using TagType = typename ResourceT::Tag;
  using LocatorType = typename ResourceT::Locator;
  static constexpr auto kTypeCodeValue = ResourceT::kTypeCodeValue;

  if (!entt::locator<LocatorType>::has_value())
  {
    return ErrorCode::LOCATOR_NOT_IMPLEMENTED;
  }
  else if (entt::locator<LocatorType>::value().load(reg, id, path))
  {
    reg.emplace<TagType>(id);
    reg.emplace<TypeCode>(id, kTypeCodeValue);
    reg.emplace<Path>(id, path);
    return std::nullopt;
  }
  else
  {
    return ErrorCode::LOAD_FAILED;
  }
}

std::optional<ErrorCode> load_any(entt::registry& reg, const entt::entity id, const Path& path, const TypeCode type)
{
  // clang-format off
  switch (type)
  {
    case TypeCode::AUDIO:
    {
      return load<Audio>(reg, id, path);
    }
    case TypeCode::TEXTURE: 
    {
      return load<Texture>(reg, id, path);
    }
    case TypeCode::TEXT:
    {
      return load<Text>(reg, id, path);
    }
  }
  // clang-format on
  return ErrorCode::UNKNOWN_LOCATOR_TYPE;
}

}  // namespace

std::ostream& operator<<(std::ostream& os, const ErrorCode error_code)
{
  switch (error_code)
  {
  case ErrorCode::UNAVAILABLE: {
    return os << "tyl::engine::core::resource::ErrorCode::UNAVAILABLE";
  }
  case ErrorCode::LOAD_FAILED: {
    return os << "tyl::engine::core::resource::ErrorCode::LOAD_FAILED";
  }
  case ErrorCode::LOCATOR_NOT_IMPLEMENTED: {
    return os << "tyl::engine::core::resource::ErrorCode::LOCATOR_NOT_IMPLEMENTED";
  }
  case ErrorCode::UNKNOWN_LOCATOR_TYPE: {
    return os << "tyl::engine::core::resource::ErrorCode::UNKNOWN_LOCATOR_TYPE";
  }
  case ErrorCode::UNKNOWN_EXTENSION: {
    return os << "tyl::engine::core::resource::ErrorCode::UNKNOWN_EXTENSION";
  }
  }
  return os << "tyl::engine::core::resource::ErrorCode::*";
}

std::ostream& operator<<(std::ostream& os, const TypeCode type_code)
{
  switch (type_code)
  {
  case TypeCode::AUDIO: {
    return os << "tyl::engine::core::resource::TypeCode::AUDIO";
  }
  case TypeCode::TEXTURE: {
    return os << "tyl::engine::core::resource::TypeCode::TEXTURE";
  }
  case TypeCode::TEXT: {
    return os << "tyl::engine::core::resource::TypeCode::TEXT";
  }
  }
  return os << "tyl::engine::core::resource::TypeCode::*";
}

expected<entt::entity, ErrorCode> create(entt::registry& reg, const Path& path, const TypeCode type)
{
  auto& rm = get_loaded_resource_map(reg);

  if (const auto itr = rm.find(path); itr != rm.end())
  {
    return itr->second;
  }
  else if (!std::filesystem::exists(path))
  {
    return unexpected{ErrorCode::UNAVAILABLE};
  }

  const auto id = reg.create();
  if (const auto error_opt = load_any(reg, id, path, type); error_opt.has_value())
  {
    reg.destroy(id);
    return unexpected{*error_opt};
  }
  else
  {
    rm.emplace(path, id);
  }
  return id;
}

expected<entt::entity, ErrorCode> create(entt::registry& reg, const Path& path)
{
  if (const auto type_code_opt = resolve_type(path); type_code_opt.has_value())
  {
    return create(reg, path, *type_code_opt);
  }
  else
  {
    return unexpected{ErrorCode::UNKNOWN_EXTENSION};
  }
}

expected<entt::entity, ErrorCode> get(entt::registry& reg, const Path& path)
{
  auto& rm = get_loaded_resource_map(reg);
  if (const auto itr = rm.find(path); itr != rm.end())
  {
    return itr->second;
  }
  else
  {
    return unexpected{ErrorCode::UNAVAILABLE};
  }
}

void release(entt::registry& reg, const Path& path)
{
  auto& rm = get_loaded_resource_map(reg);
  if (const auto itr = rm.find(path); itr != rm.end())
  {
    reg.destroy(itr->second);
    rm.erase(itr);
  }
}

void reload(entt::registry& reg, const ReloadErrorCallback& error_callback)
{
  reg.view<TypeCode, Path>().each([&rm = get_loaded_resource_map(reg), &reg, &error_callback](
                                    entt::entity id, const TypeCode& type, const Path& path) {
    // Skip is already loaded
    if (const auto itr = rm.find(path); itr != rm.end())
    {
      return;
    }
    else if (const auto error_opt = load_any(reg, id, path, type); !error_opt.has_value())
    {
      rm.emplace(path, id);
    }
    else if (error_callback)
    {
      error_callback(reg, id, path, *error_opt);
    }
  });
}

}  // namespace tyl::engine::core::resource
