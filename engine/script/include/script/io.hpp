/**
 * @copyright 2023-present Brian Cairl
 *
 * @file io.hpp
 */
#pragma once

// C++ Standard Library
#include <filesystem>

// Tyl
#include <tyl/engine/script/script.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/file_stream.hpp>

namespace tyl::engine
{

template <typename ScriptT> void save(const ScriptBase<ScriptT>& script, const std::filesystem::path& path)
{
  serialization::file_ostream ofs{path};
  serialization::binary_oarchive oar{ofs};
  script.save(oar);
}

template <typename ScriptT> bool load(ScriptBase<ScriptT>& script, const std::filesystem::path& path)
{
  if (!std::filesystem::exists(path))
  {
    return false;
  }
  serialization::file_istream ifs{path};
  serialization::binary_iarchive iar{ifs};
  script.load(iar);
  return true;
}

}  // namespace tyl::engine
