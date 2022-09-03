cc_library(
  name="render_target",
  hdrs=[
    "include/render_target.hpp",
    "include/renderable.hpp"
  ],
  srcs=[],
  strip_include_prefix="include",
  include_prefix="tyl/graphics/systems",
  deps=["//lib/common/math:geometry"],
  visibility=["//visibility:public"]
)

cc_library(
  name="sprite_renderer",
  hdrs=["include/sprite_renderer.hpp"],
  srcs=["src/sprite_renderer.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/graphics/systems",
  deps=[":render_target", "//lib/common/debug:assert", "//lib/common/math:geometry", "//lib/ecs", "//lib/graphics/device", "//lib/graphics/sprite:animation"],
  visibility=["//visibility:public"]
)
