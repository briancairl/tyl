/**
 * @copyright 2023-present Brian Cairl
 *
 * @file filesystem.cpp
 */

// C++ Standard Library
#include <filesystem>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/mem_stream.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/std/filesystem.hpp>

using namespace tyl::serialization;


TEST(StdFilesystemPath, EmptyPath)
{
  const std::filesystem::path kExpected = {};

  mem_ostream oms{};
  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"value", kExpected}));
  }

  mem_istream ims{std::move(oms)};
  {
    binary_iarchive iar{ims};
    std::filesystem::path read;
    ASSERT_NO_THROW((iar >> named{"value", read}));
    ASSERT_EQ(read, kExpected);
  }
}

TEST(StdFilesystemPath, NonEmptyPath)
{
  const std::filesystem::path kExpected{"/this/is/a/path"};

  mem_ostream oms{};
  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"value", kExpected}));
  }

  mem_istream ims{std::move(oms)};
  {
    binary_iarchive iar{ims};
    std::filesystem::path read;
    ASSERT_NO_THROW((iar >> named{"value", read}));
    ASSERT_EQ(read, kExpected);
  }
}

TEST(StdFilesystemFileType, NoneType)
{
  const std::filesystem::file_type kExpected{std::filesystem::file_type::none};

  mem_ostream oms{};
  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"value", kExpected}));
  }

  mem_istream ims{std::move(oms)};
  {
    binary_iarchive iar{ims};
    std::filesystem::file_type read;
    ASSERT_NO_THROW((iar >> named{"value", read}));
    ASSERT_EQ(read, kExpected);
  }
}

TEST(StdFilesystemFileType, OtherType)
{
  const std::filesystem::file_type kExpected{std::filesystem::file_type::socket};

  mem_ostream oms{};
  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"value", kExpected}));
  }

  mem_istream ims{std::move(oms)};
  {
    binary_iarchive iar{ims};
    std::filesystem::file_type read;
    ASSERT_NO_THROW((iar >> named{"value", read}));
    ASSERT_EQ(read, kExpected);
  }
}
