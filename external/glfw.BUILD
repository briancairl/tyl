package(default_visibility=["//visibility:public"])

cc_library(
    name="glfw",
    hdrs=glob(["include/GLFW/*.h"]),
    srcs=glob(["lib/x86_64-linux-gnu/libglfw.so"]),
    strip_include_prefix="include/GLFW",
    include_prefix="GLFW",
)
