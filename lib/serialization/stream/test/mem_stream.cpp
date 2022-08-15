/**
 * @copyright 2022-present Brian Cairl
 */

// C++ Standard Library
#include <cstring>
#include <string_view>
#include <utility>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serialization/mem_istream.hpp>
#include <tyl/serialization/mem_ostream.hpp>


TEST(FileInputStream, ReadAll)
{
  static const char* TARGET_VALUE = "this is just a sample\n";

  tyl::serialization::mem_istream ifs{[] {
    static std::string_view TARGET_VALUE_sv{TARGET_VALUE};
    std::vector<std::uint8_t> buffer{TARGET_VALUE_sv.begin(), TARGET_VALUE_sv.end()};
    return buffer;
  }()};

  char buf[23];
  ifs.read(buf, sizeof(buf));
  buf[sizeof(buf) - 1] = '\0';

  ASSERT_EQ(ifs.available(), 0UL);
  ASSERT_EQ(std::memcmp(buf, TARGET_VALUE, std::strlen(TARGET_VALUE)), 0);
}

TEST(FileInputStream, ReadTooMany)
{
  static const char* TARGET_VALUE = "this is just a sample\n";

  tyl::serialization::mem_istream ifs{[] {
    static std::string_view TARGET_VALUE_sv{TARGET_VALUE};
    std::vector<std::uint8_t> buffer{TARGET_VALUE_sv.begin(), TARGET_VALUE_sv.end()};
    return buffer;
  }()};

  char buf[23];
  ifs.read(buf, sizeof(buf) + 10);
  buf[sizeof(buf) - 1] = '\0';

  ASSERT_EQ(ifs.available(), 0UL);
  ASSERT_EQ(std::memcmp(buf, TARGET_VALUE, std::strlen(TARGET_VALUE)), 0);
}


TEST(FileOutputStream, Write)
{
  char buf[] = "this is a sample payload for write";
  tyl::serialization::mem_ostream ofs{};
  ASSERT_EQ(sizeof(buf), ofs.write(buf));
}

TEST(FileStream, WriteThenRead)
{
  char write_buf[] = "this is a sample payload for readback";
  tyl::serialization::mem_ostream ofs{};
  ASSERT_EQ(sizeof(write_buf), ofs.write(write_buf));

  char read_buf[sizeof(write_buf) * 2];
  tyl::serialization::mem_istream ifs{std::move(ofs)};
  ASSERT_EQ(ifs.read(read_buf), sizeof(write_buf));

  ASSERT_EQ(std::memcmp(write_buf, read_buf, sizeof(write_buf)), 0);
}
