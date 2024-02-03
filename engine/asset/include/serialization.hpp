/**
 * @copyright 2023-present Brian Cairl
 *
 * @file serialization.hpp
 */
#pragma once

// Tyl
#include <tyl/engine/asset/types.hpp>
#include <tyl/serialization_fwd.hpp>

namespace tyl::serialization
{

template <> struct save<binary_oarchive<file_handle_ostream>, engine::asset::Collection>
{
  void operator()(binary_oarchive<file_handle_ostream>& ar, const engine::asset::Collection& collection) const;
};

template <> struct load<binary_iarchive<file_handle_istream>, engine::asset::Collection>
{
  void operator()(binary_iarchive<file_handle_istream>& ar, engine::asset::Collection& collection) const;
};

template <> struct save<binary_oarchive<mem_ostream>, engine::asset::Collection>
{
  void operator()(binary_oarchive<mem_ostream>& ar, const engine::asset::Collection& collection) const;
};

template <> struct load<binary_iarchive<mem_istream>, engine::asset::Collection>
{
  void operator()(binary_iarchive<mem_istream>& ar, engine::asset::Collection& collection) const;
};

}  // namespace tyl::serialization
