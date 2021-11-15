/**
 * @copyright 2021-present Brian Cairl
 *
 * @file logging.cpp
 */

// C++ Standard
#include <iostream>

// SPDLOG
#include "spdlog/sinks/basic_file_sink.h"

// TYL
#include <tyl/logging/logging.hpp>

namespace tyl::logging
{

void initialize()
{
  try
  {
    spdlog::initialize_logger(spdlog::basic_logger_mt("file", "/tmp/tyl-engine-log.txt"));
  }
  catch (const spdlog::spdlog_ex& ex)
  {
    std::cout << "Log init failed: " << ex.what() << std::endl;
  }
}

}  // namespace tyl::logging
