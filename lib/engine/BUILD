cc_library(
  name="fwd",
  hdrs=["include/fwd.hpp",],
  strip_include_prefix="include",
  include_prefix="tyl/engine",
  visibility=["//visibility:private"]
)

cc_library(
  name="window",
  hdrs=[
    "include/window.hpp"
  ],
  srcs=[],
  strip_include_prefix="include",
  include_prefix="tyl/engine",
  deps=[":fwd"],
  visibility=["//visibility:public"]
)

cc_library(
  name="geometry",
  hdrs=[
    "include/geometry.hpp",
  ],
  srcs=[],
  strip_include_prefix="include",
  include_prefix="tyl/engine",
  deps=["//lib/common:matrix", "//lib/common:macros"],
  visibility=["//visibility:public"]
)

cc_library(
  name="state",
  hdrs=[
    "include/state.hpp",
  ],
  srcs=[],
  strip_include_prefix="include",
  include_prefix="tyl/engine",
  deps=["//lib/common:matrix", "//lib/common:macros"],
  visibility=["//visibility:public"]
)

cc_library(
  name="camera",
  hdrs=[
    "include/camera.hpp",
  ],
  srcs=[
    "src/camera.cpp"
  ],
  strip_include_prefix="include",
  include_prefix="tyl/engine",
  deps=["//lib/common:matrix", "//lib/common:macros", ":fwd", ":window", ":geometry"],
  visibility=["//visibility:public"]
)

cc_library(
  name="tile_map",
  hdrs=[
    "include/tile_map.hpp",
    "include/tile_map_rendering.hpp"
  ],
  srcs=[
    "src/tile_map.cpp",
    "src/tile_map_rendering.cpp"
  ],
  strip_include_prefix="include",
  include_prefix="tyl/engine",
  deps=["//lib/graphics",
        "//lib/logging",
        "//lib/common:ecs",
        "//lib/common:matrix",
        "//lib/common:macros",
        ":fwd",
        ":geometry",
        ":state",
        ":camera"],
  visibility=["//visibility:public"]
)

cc_library(
  name="app",
  hdrs=["include/app.hpp"],
  srcs=["src/app.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/engine",
  deps=["//lib/ui", "//lib/logging", ":camera", ":window"],
  visibility=["//visibility:public"]
)

