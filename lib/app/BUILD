cc_library(
  name="app",
  hdrs=["include/loop.hpp"],
  srcs=["src/loop.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/app",
  deps = [
    "//lib/common:vec",
    "//lib/graphics",
    "//lib/logging",
    "//lib/ui",
  ],
  visibility=["//visibility:public"]
)
