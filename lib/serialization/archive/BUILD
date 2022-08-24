cc_library(
  name="archive",
  hdrs=["include/fwd.hpp",
        "include/iarchive.hpp",
        "include/oarchive.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/serialization",
  deps=["//lib/serialization/types/primitives", "//lib/serialization:object"],
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

cc_library(
  name="json_archive",
  hdrs=["include/json_iarchive.hpp", "include/json_oarchive.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/serialization",
  deps=[":archive", "//lib/serialization/stream", "//lib/serialization/types/common"],
  visibility=["//visibility:public"]
)
