package(default_visibility=["//visibility:public"])

cc_library(
  name="backends",
  hdrs=glob(["backends/*opengl3*.h", "backends/*glfw*.h",]),
  copts=["-IMGUI_IMPL_OPENGL_LOADER_GLAD"],
  strip_include_prefix="backends",
  visibility=["//visibility:private"]
)

cc_library(
  name="imgui",
  hdrs=glob(["*.h"]) + [":backends"],
  copts=["-IMGUI_IMPL_OPENGL_LOADER_GLAD"],
  linkopts=["-ldl", "-lGL"],
  includes=[""],
  srcs=glob(["*.c", "*.cpp", "backends/*opengl3*.cpp", "backends/*glfw*.cpp"]),
  deps=["@glfw//:glfw", "@//core/graphics/device/glad:glad", ":backends"],
  visibility=["//visibility:public"]
)

cc_library(
  name="imgui_cpp",
  hdrs=glob(["misc/cpp/*.h"]),
  copts=["-IMGUI_IMPL_OPENGL_LOADER_GLAD"],
  linkopts=["-ldl", "-lGL"],
  includes=[""],
  srcs=glob(["misc/cpp/*.cpp"]),
  deps=[":imgui"],
  strip_include_prefix="misc/cpp",
  visibility=["//visibility:public"]
)
