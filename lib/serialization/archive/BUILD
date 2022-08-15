cc_library(
  name="archive",
  hdrs=["include/fwd.hpp", "include/iarchive.hpp", "include/oarchive.hpp", "include/object.hpp", "include/packet.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/serialization",
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="binary_archive",
  hdrs=["include/binary_iarchive.hpp", "include/binary_oarchive.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/serialization",
  deps=[":archive", "//lib/serialization/stream"],
  visibility=["//visibility:public"]
)
