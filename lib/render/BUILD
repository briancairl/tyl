cc_library(
  name="render",
  hdrs=[
    "include/camera.hpp",
    "include/fwd.hpp",
    "include/viewport.hpp"
  ],
  srcs=[
    "src/camera.cpp"
  ],
  strip_include_prefix="include",
  include_prefix="tyl/render",
  deps=["//lib/graphics", "//lib/common:matrix"],
  visibility=["//visibility:public"]
)
