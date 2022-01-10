/**
 * @copyright 2021 Brian Cairl
 */
#pragma once

// EnTT
#include <entt/entt.hpp>

namespace boost::serialization
{

template <typename ArchiveT, auto Tag>
inline void serialize(ArchiveT& ar, entt::tag<Tag>& tag, const unsigned int version)
{}

}  // namespace boost::serialization
