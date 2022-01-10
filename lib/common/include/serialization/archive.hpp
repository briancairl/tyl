/**
 * @copyright 2021-present Brian Cairl
 *
 * @file archive.hpp
 */
#pragma once

// Boost
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

namespace tyl::serialization
{

/// Input serialization archive (loading)
using IArchive = boost::archive::binary_iarchive;

/// Output serialization archive (saving)
using OArchive = boost::archive::binary_oarchive;

}  // namespace tyl::serialization
