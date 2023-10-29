cc_library(
  name="archive",
  hdrs=["include/archive_fwd.hpp",
        "include/iarchive.hpp",
        "include/oarchive.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/serialization",
  deps=["//core/serialization/primitives", "//core/serialization:object"],
  visibility=["//visibility:public"]
)

cc_library(
  name="binary_archive",
  hdrs=["include/binary_archive.hpp", "include/binary_iarchive.hpp", "include/binary_oarchive.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/serialization",
  deps=[":archive", "//core/serialization/stream"],
  visibility=["//visibility:public"]
)

cc_library(
  name="json_archive",
  hdrs=["include/json_archive.hpp", "include/json_iarchive.hpp", "include/json_oarchive.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/serialization",
  deps=[":archive", "//core/serialization/stream", "//core/serialization/primitives"],
  visibility=["//visibility:public"]
)
