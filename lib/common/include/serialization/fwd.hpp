/**
 * @copyright 2021-present Brian Cairl
 *
 * @file fwd.hpp
 */
#pragma once

namespace boost::archive
{

class binary_iarchive;
class binary_oarchive;

}  // namespace boost::archive

namespace tyl::serialization
{

/// Input serialization archive (loading)
using IArchive = boost::archive::binary_iarchive;

/// Output serialization archive (saving)
using OArchive = boost::archive::binary_oarchive;

}  // namespace tyl::serialization
