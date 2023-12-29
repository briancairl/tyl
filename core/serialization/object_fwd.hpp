/**
 * @copyright 2023-present Brian Cairl
 *
 * @file object_fwd.hpp
 */
#pragma once

namespace tyl::serialization
{

template <typename ArchiveT, typename ObjectT> struct serialize;
template <typename IArchiveT, typename ObjectT> struct load;
template <typename OArchiveT, typename ObjectT> struct save;

}  // namespace tyl::serialization
