/**
 * @copyright 2023-present Brian Cairl
 *
 * @file scene.hpp
 */
#pragma once

// C++ Standard Library

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/serialization/archive_fwd.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/stream_fwd.hpp>

namespace tyl::engine
{

/**
 * @brief Top-level scene element
 */
struct Scene
{
  /// Registry holding game data for the scene
  Registry registry;
};

}  // namespace tyl::engine

namespace tyl::serialization
{

template <> struct save<binary_oarchive<file_ostream>, engine::Scene>
{
  void operator()(binary_oarchive<file_ostream>& ar, const engine::Scene& scene) const;
};

template <> struct load<binary_iarchive<file_istream>, engine::Scene>
{
  void operator()(binary_iarchive<file_istream>& ar, engine::Scene& scene) const;
};

template <> struct save<binary_oarchive<mem_ostream>, engine::Scene>
{
  void operator()(binary_oarchive<mem_ostream>& ar, const engine::Scene& scene) const;
};

template <> struct load<binary_iarchive<mem_istream>, engine::Scene>
{
  void operator()(binary_iarchive<mem_istream>& ar, engine::Scene& scene) const;
};

}  // namespace tyl::serialization
