cc_library(
  name="common",
  hdrs=["include/common.hpp"],
  srcs=[],
  strip_include_prefix="include",
  include_prefix="tyl/graphics",
  deps=["//lib/serialization"],
  visibility=["//visibility:public"]
)

cc_library(
  name="render_target",
  hdrs=[
    "include/render_target.hpp",
  ],
  srcs=[],
  strip_include_prefix="include",
  include_prefix="tyl/graphics",
  deps=["//lib/common/math:geometry"],
  visibility=["//visibility:public"]
)

cc_library(
  name="sprite",
  hdrs=["include/sprite.hpp"],
  srcs=["src/sprite.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/graphics",
  deps=["//lib/common/debug:assert", "//lib/common/math:geometry", "//lib/common/utility:alias", "//lib/common/utility:alias_serial", "//lib/serialization", "//lib/ecs"],
  visibility=["//visibility:public"]
)

cc_library(
  name="sprite_animation",
  hdrs=["include/sprite_animation.hpp"],
  srcs=["src/sprite_animation.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/graphics",
  deps=[":common", ":render_target", ":sprite", "//lib/serialization", "//lib/ecs"],
  visibility=["//visibility:public"]
)

cc_library(
  name="sprite_rendering",
  hdrs=["include/sprite_rendering.hpp"],
  srcs=["src/sprite_rendering.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/graphics",
  deps=[":common", ":render_target", ":sprite", ":sprite_animation", ":texture_management", "//lib/common/debug:assert", "//lib/common/math:geometry", "//lib/ecs", "//lib/serialization", "//lib/graphics/device"],
  visibility=["//visibility:public"]
)

cc_library(
  name="texture_management",
  hdrs=["include/texture_management.hpp"],
  srcs=["src/texture_management.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/graphics",
  deps=["//lib/common/debug:assert", "//lib/common/math:geometry", "//lib/common/utility:alias", "//lib/common/utility:alias_serial", "//lib/ecs", "//lib/serialization", "//lib/graphics/device", "//lib/graphics/host:image"],
  visibility=["//visibility:public"]
)

cc_library(
  name="tilemap",
  hdrs=["include/tilemap.hpp"],
  srcs=["src/tilemap.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/graphics",
  deps=[
    "//lib/common/debug:assert",
    "//lib/common/math:geometry",
    "//lib/common/utility:alias",
    "//lib/common/utility:alias_serial",
    "//lib/ecs",
    "//lib/serialization",
    "//lib/graphics/device",
    "//lib/graphics/host:image",
    "//systems/graphics:tilemap"],
  visibility=["//visibility:public"]
)

cc_library(
  name="tilemap_rendering",
  hdrs=["include/tilemap_rendering.hpp"],
  srcs=["src/tilemap_rendering.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/graphics",
  deps=[
    ":common", 
    ":render_target", 
    ":tilemap",
    ":texture_management",
    "//lib/common/debug:assert",
    "//lib/common/math:geometry",
    "//lib/ecs",
    "//lib/serialization",
    "//lib/graphics/device"],
  visibility=["//visibility:public"]
)