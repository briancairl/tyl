/**
 * @copyright 2023-present Brian Cairl
 *
 * @file io.hpp
 */
#pragma once

// C++ Standard Library
#include <filesystem>

// Tyl
#include <tyl/engine/widget/widget.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/file_stream.hpp>

namespace tyl::engine
{

template <typename WidgetT> void save(const WidgetBase<WidgetT>& widget, const std::filesystem::path& path)
{
  serialization::file_ostream ofs{path};
  serialization::binary_oarchive oar{ofs};
  widget.save(oar);
}

template <typename WidgetT> bool load(WidgetBase<WidgetT>& widget, const std::filesystem::path& path)
{
  if (!std::filesystem::exists(path))
  {
    return false;
  }
  serialization::file_istream ifs{path};
  serialization::binary_iarchive iar{ifs};
  widget.load(iar);
  return true;
}

}  // namespace tyl::engine
