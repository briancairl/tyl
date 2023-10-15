/**
 * @copyright 2022-present Brian Cairl
 */

// C++ Standard Library
#include <cstring>
#include <stdexcept>
#include <utility>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serialization/file_istream.hpp>
#include <tyl/serialization/file_ostream.hpp>


TEST(FileInputStream, CannotOpenFile)
{
  ASSERT_THROW((tyl::serialization::file_istream{"not-a-file.bin", {.nobuf = true}}), std::runtime_error);
}


TEST(FileInputStream, MoveCTor)
{
  tyl::serialization::file_istream ifs{"core/serialization/stream/test/resources/file_stream.dat", {.nobuf = true}};

  ASSERT_EQ(ifs.available(), 22UL);

  tyl::serialization::file_istream ifs_move{std::move(ifs)};

  ASSERT_EQ(ifs.available(), 0UL);
  ASSERT_EQ(ifs_move.available(), 22UL);
}

TEST(FileInputStream, ReadAll)
{
  tyl::serialization::file_istream ifs{"core/serialization/stream/test/resources/file_stream.dat", {.nobuf = true}};

  char buf[23];
  ifs.read(buf, sizeof(buf));
  buf[sizeof(buf) - 1] = '\0';

  ASSERT_EQ(ifs.available(), 0UL);

  static const char* TARGET_VALUE = "this is just a sample\n";
  ASSERT_EQ(std::memcmp(buf, TARGET_VALUE, std::strlen(TARGET_VALUE)), 0);
}

TEST(FileInputStream, ReadTooMany)
{
  tyl::serialization::file_istream ifs{"core/serialization/stream/test/resources/file_stream.dat", {.nobuf = true}};

  char buf[23];
  ifs.read(buf, sizeof(buf) + 10);
  buf[sizeof(buf) - 1] = '\0';

  ASSERT_EQ(ifs.available(), 0UL);

  static const char* TARGET_VALUE = "this is just a sample\n";
  ASSERT_EQ(std::memcmp(buf, TARGET_VALUE, std::strlen(TARGET_VALUE)), 0);
}


TEST(FileOutputStream, CreateFileOnAppend)
{
  ASSERT_NO_THROW((tyl::serialization::file_ostream{"ostream-append-not-a-file.bin", {.append = true}}));
}


TEST(FileOutputStream, CreateFileOnWrite)
{
  ASSERT_NO_THROW((tyl::serialization::file_ostream{"ostream-write-not-a-file.bin", {.append = false}}));
}


TEST(FileOutputStream, Write)
{
  char buf[] = "this is a sample payload for write";
  tyl::serialization::file_ostream ofs{"write.bin"};
  ASSERT_EQ(sizeof(buf), ofs.write(buf));
}

TEST(FileStream, WriteThenRead)
{
  char write_buf[] = "this is a sample payload for readback";
  tyl::serialization::file_ostream ofs{"readback.bin"};
  ASSERT_EQ(sizeof(write_buf), ofs.write(write_buf));

  char read_buf[sizeof(write_buf) * 2];
  tyl::serialization::file_istream ifs{"readback.bin"};
  ASSERT_EQ(ifs.read(read_buf), sizeof(write_buf));

  ASSERT_EQ(std::memcmp(write_buf, read_buf, sizeof(write_buf)), 0);
}
