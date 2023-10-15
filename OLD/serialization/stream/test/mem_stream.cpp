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


TEST(MemInputStream, ReadAll)
{
  static const char* TARGET_VALUE = "this is just a sample\n";

  tyl::serialization::mem_istream ims{[] {
    static std::string_view TARGET_VALUE_sv{TARGET_VALUE};
    std::vector<std::uint8_t> buffer{TARGET_VALUE_sv.begin(), TARGET_VALUE_sv.end()};
    return buffer;
  }()};

  ASSERT_EQ(ims.available(), 22UL);

  char buf[23];
  ims.read(buf, sizeof(buf) - 1);
  buf[sizeof(buf) - 1] = '\0';

  ASSERT_EQ(ims.available(), 0UL);
  ASSERT_EQ(std::memcmp(buf, TARGET_VALUE, std::strlen(TARGET_VALUE)), 0);
}

TEST(MemInputStream, ReadTooMany)
{
  static const char* TARGET_VALUE = "this is just a sample\n";

  tyl::serialization::mem_istream ims{[] {
    static std::string_view TARGET_VALUE_sv{TARGET_VALUE};
    std::vector<std::uint8_t> buffer{TARGET_VALUE_sv.begin(), TARGET_VALUE_sv.end()};
    return buffer;
  }()};

  ASSERT_EQ(ims.available(), 22UL);

  char buf[23];
  ims.read(buf, sizeof(buf) + 10);
  buf[sizeof(buf) - 1] = '\0';

  ASSERT_EQ(ims.available(), 0UL);
  ASSERT_EQ(std::memcmp(buf, TARGET_VALUE, std::strlen(TARGET_VALUE)), 0);
}


TEST(MemOutputStream, Write)
{
  char buf[] = "this is a sample payload for write";
  tyl::serialization::mem_ostream oms{};
  ASSERT_EQ(sizeof(buf), oms.write(buf));
}

TEST(MemStream, WriteThenRead)
{
  char write_buf[] = "this is a sample payload for readback";
  tyl::serialization::mem_ostream oms{};
  ASSERT_EQ(sizeof(write_buf), oms.write(write_buf));

  char read_buf[sizeof(write_buf) * 2];
  tyl::serialization::mem_istream ims{std::move(oms)};
  ASSERT_EQ(ims.read(read_buf), sizeof(write_buf));

  ASSERT_EQ(std::memcmp(write_buf, read_buf, sizeof(write_buf)), 0);
}
