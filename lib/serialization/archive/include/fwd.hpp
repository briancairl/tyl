/**
 * @copyright 2022-present Brian Cairl
 *
 * @file fwd.hpp
 */
#pragma once

namespace tyl::serialization
{

template <typename IArchiveT> class iarchive;
template <typename OArchiveT> class oarchive;
template <typename IStreamT> class binary_iarchive;
template <typename OStreamT> class binary_oarchive;

}  // namespace tyl::serialization