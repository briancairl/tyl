package(default_visibility=["//visibility:public"])

cc_library(
  name="gl3w",
  hdrs=glob(["examples/libs/gl3w/GL/*.h"]),
  srcs=glob(["examples/libs/gl3w/GL/*.c"]),
  strip_include_prefix="examples/libs/gl3w",
  visibility=["//visibility:public"]
)

cc_library(
  name="backends",
  hdrs=glob(["backends/*opengl3*.h", "backends/*glfw*.h",]),
  copts=["-DIMGUI_IMPL_OPENGL_LOADER_GL3W"],
  strip_include_prefix="backends",
  visibility=["//visibility:private"]
)

cc_library(
  name="imgui",
  hdrs=glob(["*.h"]) + [":backends"],
  copts=["-DIMGUI_IMPL_OPENGL_LOADER_GL3W"],
  linkopts=["-ldl", "-lGL"],
  includes=[""],
  srcs=glob(["*.c", "*.cpp", "backends/*opengl3*.cpp", "backends/*glfw*.cpp"]),
  deps=["@glfw//:glfw", ":gl3w", ":backends"],
  visibility=["//visibility:public"]
)

cc_library(
  name="imgui_cpp",
  hdrs=glob(["misc/cpp/*.h"]),
  copts=["-DIMGUI_IMPL_OPENGL_LOADER_GL3W"],
  linkopts=["-ldl", "-lGL"],
  includes=[""],
  srcs=glob(["misc/cpp/*.cpp"]),
  deps=[":imgui"],
  strip_include_prefix="misc/cpp",
  visibility=["//visibility:public"]
)
