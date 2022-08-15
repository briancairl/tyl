package(default_visibility=["//visibility:public"])

cc_library(
    name="glfw",
    hdrs=[
    	"include/GLFW/glfw3.h",
    	"include/GLFW/glfw3native.h"
    ],
    srcs=glob(["lib/x86_64-linux-gnu/libglfw.so*"]),
    strip_include_prefix="include/GLFW",
    include_prefix="GLFW",
)
