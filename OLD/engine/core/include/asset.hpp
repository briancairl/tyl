/**
 * @copyright 2023-present Brian Cairl
 *
 * @file asset.hpp
 */
#pragma once

// C++ Standard Library
#include <filesystem>
#include <iosfwd>

// Tyl
#include <tyl/async/worker_pool.hpp>
#include <tyl/utility/expected.hpp>

namespace tyl::engine::asset
{

using Path = std::filesystem::path;

enum class ErrorCode
{
  kUnavailable,
  kExists,
  kLoadFailed,
  kUnknownExtension,
  kUnimplementedLoader
};

enum class TypeCode
{
  kAudio,
  kTexture,
  kText
};

template <TypeCode CODE> struct TypeTag
{
  static constexpr TypeCode type_code = CODE;
};

using AudioTag = TypeTag<TypeCode::kAudio>;
using TextureTag = TypeTag<TypeCode::kTexture>;
using TextTag = TypeTag<TypeCode::kText>;

struct IsLoading
{};

expected<entt::entity, ErrorCode> load(async::worker_pool& pool, Registry&, const Path& path, const TypeCode type);

expected<entt::entity, ErrorCode> load(async::worker_pool& pool, Registry&, const Path& path);

bool release(entt::registry& reg, const Path& path);

bool release(entt::registry& reg, const entt::entity);

void update(entt::registry& reg);

expected<entt::entity, ErrorCode> get(entt::registry& reg, const Path& path);

std::ostream& operator<<(std::ostream& os, const ErrorCode error_code);

std::ostream& operator<<(std::ostream& os, const TypeCode type_code);

}  // namespace tyl::engine::asset
