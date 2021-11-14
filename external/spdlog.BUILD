licenses(['notice'])

cc_library(
    name="spdlog",
    hdrs=glob(["include/spdlog/**/*.h",]),
    srcs=glob(["src/*.cpp",]),
    copts=["-DSPDLOG_COMPILED_LIB"],
    strip_include_prefix="include",
    include_prefix="",
    visibility=["//visibility:public"],
)
