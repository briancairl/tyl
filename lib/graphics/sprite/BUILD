cc_library(
  name="animation",
  hdrs=["include/animation.hpp"],
  srcs=["src/animation.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/graphics/sprite",
  deps=[":spritesheet", "//lib/common/debug:assert", "//lib/graphics/device"],
  visibility=["//visibility:public"]
)

cc_library(
  name="spritesheet",
  hdrs=["include/spritesheet.hpp"],
  srcs=["src/spritesheet.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/graphics/sprite",
  deps=["//lib/common/debug:assert", "//lib/common/math:geometry", "//lib/graphics/device"],
  visibility=["//visibility:public"]
)
