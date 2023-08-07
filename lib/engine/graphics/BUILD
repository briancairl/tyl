cc_library(
  name="types",
  hdrs=["include/types.hpp"],
  srcs=[],
  strip_include_prefix="include",
  include_prefix="tyl/engine/graphics",
  deps=["//lib/common/math:geometry"],
  visibility=["//visibility:public"]
)

cc_library(
  name="camera",
  hdrs=["include/camera.hpp"],
  srcs=[],
  strip_include_prefix="include",
  include_prefix="tyl/engine/graphics",
  deps=["//lib/common/math:geometry"],
  visibility=["//visibility:public"]
)

cc_library(
  name="primitives_renderer",
  hdrs=["include/primitives_renderer.hpp"],
  srcs=["src/primitives_renderer.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/engine/graphics",
  deps=[":camera", ":types", "@entt", "//lib/graphics/device", "//lib/common/utility:expected"],
  visibility=["//visibility:public"]
)
