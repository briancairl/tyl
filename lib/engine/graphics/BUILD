cc_library(
  name="types",
  hdrs=["include/types.hpp"],
  srcs=[],
  strip_include_prefix="include",
  include_prefix="tyl/graphics/engine",
  deps=["//lib/common/math:geometry"],
  visibility=["//visibility:public"]
)

cc_library(
  name="primitives_renderer",
  hdrs=["include/primitives_renderer.hpp"],
  srcs=["src/primitives_renderer.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/graphics/engine",
  deps=[":types", "@entt", "//lib/graphics/device"],
  visibility=["//visibility:public"]
)
