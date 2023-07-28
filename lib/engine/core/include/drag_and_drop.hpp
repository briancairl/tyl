/**
 * @copyright 2023-present Brian Cairl
 *
 * @file drag_and_drop.hpp
 */
#pragma once

// C++ Standard Library
#include <filesystem>
#include <vector>

namespace tyl::engine::core
{

struct DragAndDropData
{
  std::vector<std::filesystem::path> paths;
};

}  // namespace tyl::engine::core
