cc_library(
  name="archive",
  hdrs=["include/iarchive.hpp", "include/oarchive.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/serialization",
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="binary_archive_detail",
  hdrs=["include/binary_archive/packet.hpp"],
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
  deps=[":binary_archive_detail", ":archive", "//lib/serialization/stream"],
  visibility=["//visibility:public"]
)
