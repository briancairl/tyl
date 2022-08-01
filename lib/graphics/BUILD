cc_library(
  name="image",
  hdrs=["include/image.hpp"],
  srcs=["src/image.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/graphics",
  deps=[":texture", "//lib/common:vec", "//lib/device/graphics:graphics", "@stb//:stb",],
  visibility=["//visibility:public"]
)


cc_library(
  name="texture",
  hdrs=["include/texture.hpp"],
  srcs=[],
  strip_include_prefix="include",
  include_prefix="tyl/graphics",
  deps=["//lib/common:vec", "//lib/device/graphics:graphics", "@stb//:stb",],
  visibility=["//visibility:public"]
)


cc_library(
  name="texture_tilesheet_lookup",
  hdrs=["include/texture_tilesheet_lookup.hpp"],
  srcs=["src/texture_tilesheet_lookup.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/graphics",
  deps=[":texture", "//lib/common:assert", "//lib/common:vec", "//lib/common:rect", "//lib/device/graphics:graphics"],
  visibility=["//visibility:public"]
)
