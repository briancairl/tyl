COMMON_LINKOPTS = ["-lstdc++fs"]

config_setting(
    name = "debug",
    values = {"compilation_mode": "dbg"},
)

config_setting(
    name = "sanitize_only",
    values = {"compilation_mode": "fastbuild"},
)

MODE_COPTS = select({
    ":debug": ["-fsanitize=address", "-fsanitize-address-use-after-scope", "-DADDRESS_SANITIZER", "-g", "-fno-omit-frame-pointer", "-O0"],
    ":sanitize_only": ["-fsanitize=address", "-DADDRESS_SANITIZER", "-g", "-fno-omit-frame-pointer", "-O1", "-DNDEBUG"],
    "//conditions:default": ["-O3", "-DNDEBUG"],
})

MODE_LINKOPTS = select({
    ":debug": COMMON_LINKOPTS + ["-fsanitize=address", "-static-libasan"],
    ":sanitize_only": COMMON_LINKOPTS + ["-fsanitize=address", "-static-libasan"],
    "//conditions:default": COMMON_LINKOPTS + [],
})

cc_binary(
    name="map-generator",
    srcs=["map-generator.cpp"],
    deps=["//lib/graphics", "//lib/ui", "//lib/wfc", "@imgui//:imgui_cpp"],
    visibility=["//visibility:public"],
    linkopts=MODE_LINKOPTS,
    copts=MODE_COPTS,
    data=[]
)


cc_binary(
    name="engine",
    srcs=["engine.cpp"],
    deps=["//lib/graphics", "//lib/engine/app"],
    visibility=["//visibility:public"],
    linkopts=MODE_LINKOPTS,
    copts=MODE_COPTS,
    data=["resources/test/poke-gba.png"]
)