/**
 * @copyright 2023-present Brian Cairl
 *
 * @file resource.hpp
 */
#pragma once

// C++ Standard Library
#include <filesystem>
#include <functional>
#include <iosfwd>
#include <optional>

// Entt
#include <entt/entt.hpp>

// Tyl
#include <tyl/core/engine/typeinfo.hpp>
#include <tyl/utility/expected.hpp>

namespace tyl::core::resource
{

using Path = std::filesystem::path;

enum class ErrorCode
{
  UNAVAILABLE,
  LOAD_FAILED,
  LOCATOR_NOT_IMPLEMENTED,
  UNKNOWN_LOCATOR_TYPE,
  UNKNOWN_EXTENSION,
};

std::ostream& operator<<(std::ostream& os, const ErrorCode error_code);

enum class TypeCode
{
  AUDIO,
  TEXTURE,
  TEXT
};

std::ostream& operator<<(std::ostream& os, const TypeCode type_code);

template <typename ResourceT, TypeCode TYPE> struct Resource
{
  static constexpr TypeCode kTypeCodeValue = TYPE;
  struct Tag
  {};
  struct Locator
  {
    virtual ~Locator() = default;
    virtual bool load(entt::registry& reg, const entt::entity id, const Path& path) const = 0;
  };
};

struct Audio final : Resource<Audio, TypeCode::AUDIO>
{};

struct Texture final : Resource<Texture, TypeCode::TEXTURE>
{};

struct Text final : Resource<Text, TypeCode::TEXT>
{};

expected<entt::entity, ErrorCode> create(entt::registry& reg, const Path& path, const TypeCode type);

expected<entt::entity, ErrorCode> create(entt::registry& reg, const Path& path);

expected<entt::entity, ErrorCode> get(entt::registry& reg, const Path& path);

using ReloadErrorCallback = std::function<void(entt::registry& reg, const entt::entity, const Path&, const ErrorCode)>;

void release(entt::registry& reg, const Path& path);

void reload(entt::registry& reg, const ReloadErrorCallback& error_callback = nullptr);

}  // namespace tyl::core::resource
