package(default_visibility=["//visibility:public"])

cc_library(
    name="openal",
    hdrs=glob(["include/AL/*.h"]),
    srcs=[
      "lib/x86_64-linux-gnu/libaudio.so",
      "lib/x86_64-linux-gnu/libopenal.so"
    ],
    strip_include_prefix="include",
)
