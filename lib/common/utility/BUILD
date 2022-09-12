cc_library(
  name="alias",
  hdrs=[
    "include/alias.hpp",
  ],
  strip_include_prefix="include",
  include_prefix="tyl/utility",
  visibility=["//visibility:public"]
)

cc_library(
  name="bits",
  hdrs=["include/bits.hpp",],
  strip_include_prefix="include",
  include_prefix="tyl/utility",
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="dynamic_bitset",
  hdrs=["include/dynamic_bitset.hpp",],
  strip_include_prefix="include",
  include_prefix="tyl/utility",
  deps=[":bits"],
  visibility=["//visibility:public"]
)


