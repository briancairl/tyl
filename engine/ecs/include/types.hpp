/*
 * @copyright 2023-present Brian Cairl
 *
 * @file types.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs.hpp>

namespace tyl::engine
{

using Registry = tyl::Registry;
using EntityID = tyl::EntityID;

template <typename T> using Reference = tyl::Reference<T>;

}  // namespace tyl::engine
