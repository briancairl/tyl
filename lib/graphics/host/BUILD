cc_library(
  name="image",
  hdrs=["include/image.hpp"],
  srcs=["src/image.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/graphics/host",
  deps=["//lib/common/math:geometry", "//lib/graphics/device", "@stb//:stb",],
  visibility=["//visibility:public"]
)

cc_library(
  name="image_serial",
  hdrs=[
    "serial/image.hpp",
  ],
  strip_include_prefix="serial",
  include_prefix="tyl/serial/graphics/host",
  deps=[
    ":image",
    "//lib/serialization:object",
    "//lib/serialization/primitives"
  ],
  visibility=["//visibility:public"]
)
