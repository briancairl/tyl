cc_library(
  name="adaptors",
  hdrs=[
    "include/alias.hpp",
    "include/reference.hpp",
  ],
  strip_include_prefix="include",
  include_prefix="tyl/common",
  visibility=["//visibility:public"]
)

cc_library(
  name="assert",
  hdrs=["include/assert.hpp",],
  strip_include_prefix="include",
  include_prefix="tyl/common",
  visibility=["//visibility:public"]
)

cc_library(
  name="bits",
  hdrs=["include/bits.hpp",],
  strip_include_prefix="include",
  include_prefix="tyl/common",
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="dynamic_bitset",
  hdrs=["include/dynamic_bitset.hpp",],
  strip_include_prefix="include",
  include_prefix="tyl/common",
  deps=[":bits"],
  visibility=["//visibility:public"]
)

cc_library(
  name="vec",
  hdrs=["include/vec.hpp",],
  strip_include_prefix="include",
  include_prefix="tyl/common",
  deps=["@eigen"],
  visibility=["//visibility:public"]
)

cc_library(
  name="rect",
  hdrs=["include/rect.hpp",],
  strip_include_prefix="include",
  include_prefix="tyl/common",
  deps=[":vec"],
  visibility=["//visibility:public"]
)
