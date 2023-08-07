/**
 * @copyright 2023-present Brian Cairl
 *
 * @file asset.cpp
 */

// C++ Standard Library
#include <fstream>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

// Tyl
#include <tyl/async/non_blocking_future.hpp>
#include <tyl/async/worker_pool.hpp>
#include <tyl/engine/core/asset.hpp>
#include <tyl/engine/core/resources.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>

namespace tyl::engine::core::asset
{
namespace
{

struct LoadingTag
{};

struct HashPath
{
  std::size_t operator()(const Path& p) const { return std::filesystem::hash_value(p); }
};

expected<TypeCode, ErrorCode> resolve_type(const Path& path)
{
  static const std::unordered_set<Path, HashPath> kTextureExtensions{".png", ".jpg", ".jpeg"};
  static const std::unordered_set<Path, HashPath> kAudioExtensions{".wav", ".mp3"};
  static const std::unordered_set<Path, HashPath> kTextExtensions{".txt", ".glsl"};

  const auto ext = path.extension();
  if (kTextureExtensions.count(ext))
  {
    return TypeCode::kTexture;
  }
  else if (kAudioExtensions.count(ext))
  {
    return TypeCode::kAudio;
  }
  else if (kTextExtensions.count(ext))
  {
    return TypeCode::kText;
  }
  return unexpected{ErrorCode::kUnknownExtension};
}

using LoadedAssetMap = std::unordered_map<Path, entt::entity, HashPath>;

LoadedAssetMap& get_loaded_asset_map(entt::registry& reg)
{
  auto& ctx = reg.ctx();
  if (auto* const rm = ctx.find<LoadedAssetMap>(); rm == nullptr)
  {
    return ctx.emplace<LoadedAssetMap>();
  }
  else
  {
    return *rm;
  }
}

bool is_asset_loaded(entt::registry& reg, const Path& path)
{
  const auto& rm = get_loaded_asset_map(reg);
  return rm.find(path) != rm.end();
}

template <TypeCode kTypeCode> struct Asset;

template <> struct Asset<TypeCode::kTexture>
{
  using type = graphics::device::Texture;
  using loaded_type = graphics::host::Image;

  [[nodiscard]] static expected<loaded_type, ErrorCode> load(const Path& path)
  {
    if (auto image_or_error = graphics::host::Image::load(path.string().c_str()); image_or_error.has_value())
    {
      return std::move(image_or_error).value();
    }
    return unexpected{ErrorCode::kLoadFailed};
  }

  static void submit(entt::registry& reg, const entt::entity id, loaded_type&& loaded)
  {
    reg.emplace<type>(id, loaded.texture());
  }
};

template <> struct Asset<TypeCode::kText>
{
  using type = std::string;
  using loaded_type = std::string;

  [[nodiscard]] static expected<loaded_type, ErrorCode> load(const Path& path)
  {
    if (std::ifstream ifs{path}; ifs.is_open())
    {
      std::stringstream ss;
      ss << ifs.rdbuf();
      return ss.str();
    }
    return unexpected{ErrorCode::kLoadFailed};
  }

  constexpr static void submit(entt::registry& reg, const entt::entity id, loaded_type&& loaded) {}
};

template <> struct Asset<TypeCode::kAudio>
{
  using type = int;
  using loaded_type = int;

  [[nodiscard]] static expected<loaded_type, ErrorCode> load(const Path& path)
  {
    return unexpected{ErrorCode::kUnimplementedLoader};
  }

  constexpr static void submit(entt::registry& reg, const entt::entity id, loaded_type&& loaded) {}
};

template <TypeCode kTypeCode> using asset_t = typename Asset<kTypeCode>::loaded_type;

template <TypeCode kTypeCode> using asset_or_error_t = expected<asset_t<kTypeCode>, ErrorCode>;

template <TypeCode kTypeCode> using loading_asset_or_error_t = async::non_blocking_future<asset_or_error_t<kTypeCode>>;

template <TypeCode kTypeCode, typename WorkGroupT, typename WorkQueueT, typename WorkPoolOptionsT>
loading_asset_or_error_t<kTypeCode>
load(async::worker_pool_base<WorkGroupT, WorkQueueT, WorkPoolOptionsT>& wp, const Path& path)
{
  return async::post_nonblocking(wp, [path] { return Asset<kTypeCode>::load(path); });
}

template <TypeCode kTypeCode> void load(Resources& resources, const entt::entity id, const Path& path)
{
  resources.registry.template emplace<loading_asset_or_error_t<kTypeCode>>(
    id, load<kTypeCode>(resources.worker_pool, path));
  resources.registry.template emplace<TypeCode>(id, kTypeCode);
  resources.registry.template emplace<TypeTag<kTypeCode>>(id);
  resources.registry.template emplace<Path>(id, path);
  resources.registry.template emplace<IsLoading>(id);
}

template <TypeCode kTypeCode> void try_submit_loaded_assets(entt::registry& reg)
{
  auto view = reg.template view<loading_asset_or_error_t<kTypeCode>>();

  view.each([&reg](const entt::entity id, loading_asset_or_error_t<kTypeCode>& loading_asset) {
    if (!loading_asset.valid())
    {
      return;
    }
    else if (auto polled_future_value_or_error = loading_asset.get(); !polled_future_value_or_error.has_value())
    {
      return;
    }
    else if (auto value_or_error = std::move(*polled_future_value_or_error); value_or_error.has_value())
    {
      Asset<kTypeCode>::submit(reg, id, std::move(value_or_error).value());
      reg.template remove<loading_asset_or_error_t<kTypeCode>>(id);
      reg.template remove<IsLoading>(id);
      get_loaded_asset_map(reg).emplace(reg.template get<Path>(id), id);
    }
    else
    {
      reg.template emplace<ErrorCode>(id, value_or_error.error());
    }
  });
}

}  // namespace

expected<entt::entity, ErrorCode> load(Resources& resources, const Path& path, const TypeCode type)
{
  if (is_asset_loaded(resources.registry, path))
  {
    return unexpected{ErrorCode::kExists};
  }
  else if (!std::filesystem::exists(path))
  {
    return unexpected{ErrorCode::kUnavailable};
  }

  const auto id = resources.registry.create();

  switch (type)
  {
  case TypeCode::kAudio: {
    load<TypeCode::kAudio>(resources, id, path);
    break;
  }
  case TypeCode::kTexture: {
    load<TypeCode::kTexture>(resources, id, path);
    break;
  }
  case TypeCode::kText: {
    load<TypeCode::kText>(resources, id, path);
    break;
  }
  }
  return id;
}

expected<entt::entity, ErrorCode> load(Resources& resources, const Path& path)
{
  if (const auto type_or_error = resolve_type(path); type_or_error.has_value())
  {
    return load(resources, path, *type_or_error);
  }
  else
  {
    return unexpected{type_or_error.error()};
  }
}

bool release(entt::registry& reg, const Path& path)
{
  auto& rm = get_loaded_asset_map(reg);
  if (const auto itr = rm.find(path); itr != rm.end())
  {
    reg.destroy(itr->second);
    rm.erase(itr);
    return true;
  }
  return false;
}

bool release(entt::registry& reg, const entt::entity id) { return reg.valid(id) and release(reg, reg.get<Path>(id)); }

void update(entt::registry& reg)
{
  try_submit_loaded_assets<TypeCode::kTexture>(reg);
  try_submit_loaded_assets<TypeCode::kText>(reg);
}

expected<entt::entity, ErrorCode> get(entt::registry& reg, const Path& path)
{
  auto& rm = get_loaded_asset_map(reg);
  if (const auto itr = rm.find(path); itr != rm.end())
  {
    return itr->second;
  }
  return unexpected{ErrorCode::kUnavailable};
}

std::ostream& operator<<(std::ostream& os, const ErrorCode error_code)
{
  switch (error_code)
  {
  case ErrorCode::kUnavailable: {
    return os << "tyl::engine::core::asset::ErrorCode::kUnavailable";
  }
  case ErrorCode::kExists: {
    return os << "tyl::engine::core::asset::ErrorCode::kExists";
  }
  case ErrorCode::kLoadFailed: {
    return os << "tyl::engine::core::asset::ErrorCode::kLoadFailed";
  }
  case ErrorCode::kUnimplementedLoader: {
    return os << "tyl::engine::core::asset::ErrorCode::kUnimplementedLoader";
  }
  case ErrorCode::kUnknownExtension: {
    return os << "tyl::engine::core::asset::ErrorCode::kUnknownExtension";
  }
  }
  return os << "tyl::engine::core::asset::ErrorCode::*";
}

std::ostream& operator<<(std::ostream& os, const TypeCode type_code)
{
  switch (type_code)
  {
  case TypeCode::kAudio: {
    return os << "tyl::engine::core::asset::TypeCode::kAudio";
  }
  case TypeCode::kTexture: {
    return os << "tyl::engine::core::asset::TypeCode::kTexture";
  }
  case TypeCode::kText: {
    return os << "tyl::engine::core::asset::TypeCode::kText";
  }
  }
  return os << "tyl::engine::core::asset::TypeCode::*";
}

}  // namespace tyl::engine::core::asset
