/**
 * @copyright 2022-present Brian Cairl
 *
 * @file ecs.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs/ecs.hpp>

namespace tyl::ecs
{

/**
 * @brief A proxy object used for serializing a registry
 */
template <typename... ComponentTs> struct writer
{
  explicit writer(const registry& r) : ref{&r} {}
  constexpr const registry* operator->() const { return ref; }
  constexpr const registry& operator*() const { return *ref; }

private:
  const registry* const ref;
};

/**
 * @brief A proxy object used for deserializing a registry
 */
template <typename... ComponentTs> struct reader
{
  explicit reader(registry& r) : ref{&r} {}
  constexpr registry* operator->() { return ref; }
  constexpr registry& operator*() { return *ref; }

private:
  registry* const ref;
};

}  // namespace tyl::ecs

// Tyl
#include <tyl/ecs/load.hpp>
#include <tyl/ecs/save.hpp>
