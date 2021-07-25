COMMON_LINKOPTS = ["-lstdc++fs"]

config_setting(
    name = "sanitize_full",
    values = {"compilation_mode": "dbg"},
)

config_setting(
    name = "sanitize_fast",
    values = {"compilation_mode": "fastbuild"},
)

MODE_COPTS = select({
    ":sanitize_full": ["-fsanitize=address", "-fsanitize-address-use-after-scope", "-DADDRESS_SANITIZER", "-g", "-fno-omit-frame-pointer", "-O0"],
    ":sanitize_fast": ["-fsanitize=address", "-DADDRESS_SANITIZER", "-g", "-fno-omit-frame-pointer", "-O1"],
    "//conditions:default": ["-O3"],
})

MODE_LINKOPTS = select({
    ":sanitize_full": COMMON_LINKOPTS + ["-fsanitize=address", "-static-libasan"],
    ":sanitize_fast": COMMON_LINKOPTS + ["-fsanitize=address", "-static-libasan"],
    "//conditions:default": COMMON_LINKOPTS + [],
})

cc_binary(
    name="main",
    srcs=["main.cpp"],
    deps=["//lib/graphics", "//lib/ui", "@imgui//:imgui_cpp"],
    visibility=["//visibility:public"],
    linkopts=MODE_LINKOPTS,
    copts=MODE_COPTS,
    data=[]
)
