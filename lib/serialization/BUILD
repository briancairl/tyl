cc_library(
  name="stream",
  hdrs=["include/istream.hpp", "include/ostream.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/serialization",
  deps=[],
  visibility=["//visibility:private"]
)

cc_library(
  name="file_stream",
  hdrs=[
    "include/file_istream.hpp",
    "include/file_ostream.hpp"
  ],
  srcs=[
    "src/file_istream.cpp",
    "src/file_ostream.cpp"
  ],
  strip_include_prefix="include",
  include_prefix="tyl/serialization",
  deps=[":stream"],
  visibility=["//visibility:public"]
)

cc_library(
  name="archive",
  hdrs=["include/iarchive.hpp", "include/oarchive.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/serialization",
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="binary",
  hdrs=["include/binary/payload.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/serialization",
  deps=[],
  visibility=["//visibility:private"]
)

cc_library(
  name="binary_archive",
  hdrs=["include/binary_iarchive.hpp", "include/binary_oarchive.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/serialization",
  deps=[":archive", ":binary", ":stream"],
  visibility=["//visibility:public"]
)
