### Graphics declarations ###

cc_library(
  name="graphics_hdrs",
  hdrs=[
    "include/typecode.hpp",
    "include/typedef.hpp",
    "include/constants.hpp",
    "include/debug.hpp",
    "include/fwd.hpp",
    "include/shader.hpp",
    "include/texture.hpp",
    "include/vertex_buffer.hpp",
  ],
  strip_include_prefix="include",
  include_prefix="tyl/graphics/device",
  deps=["//lib/common/utility:expected"],
  visibility=["//visibility:private"]
)

### OpenAL implemenatation ###

cc_library(
  name="graphics_impl__opengl_internal",
  hdrs=["src/opengl/gl.inl"],
  strip_include_prefix="src/opengl",
  include_prefix="tyl/graphics/device",
  deps=[":graphics_hdrs", "//lib/graphics/device/glad:glad", "@glfw//:glfw",],
  visibility=["//visibility:private"]
)

config_setting(
    name = "debug",
    values = {"compilation_mode": "dbg"},
)

graphics_impl__opengl_debug_selector = select({
    ":debug": ["src/opengl/debug.cpp"],
    "//conditions:default": ["src/opengl/debug_null.cpp"],
})

cc_library(
  name="graphics_impl__opengl",
  srcs=[
    "src/opengl/shader.cpp",
    "src/opengl/texture.cpp",
    "src/opengl/vertex_buffer.cpp",
  ] + graphics_impl__opengl_debug_selector,
  deps=[":graphics_impl__opengl_internal", ":graphics_hdrs", "//lib/common/debug:assert"],
  visibility=["//visibility:private"]
)

### Graphics implementation ###

alias(
  name="device",
  actual=":graphics_impl__opengl",
  visibility=["//visibility:public"]
)
