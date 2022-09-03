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

cc_library(
  name="sprite_serial",
  hdrs=[
    "serial/animation.hpp",
    "serial/spritesheet.hpp",
  ],
  strip_include_prefix="serial",
  include_prefix="tyl/serial/graphics/sprite",
  deps=[
    "//lib/common/std:std_serial",
    "//lib/common/math:geometry_serial",
    "//lib/graphics/sprite:animation",
    "//lib/graphics/sprite:spritesheet",
    "//lib/serialization:object",
    "//lib/serialization/primitives"
  ],
  visibility=["//visibility:public"]
)
