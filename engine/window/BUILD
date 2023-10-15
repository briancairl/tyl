cc_library(
  name="window",
  hdrs=["include/keyboard.hpp", "include/window.hpp"],
  srcs=["src/window_glfw.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/engine",
  deps=[
    "//core/common",
    "//core/math",
    "//core/graphics/device",
    "@imgui//:imgui",
  ],
  visibility=["//visibility:public"]
)