TEST_COPTS = ["-Iexternal/googletest/googletest/include", "-fsanitize=address", "-fsanitize-address-use-after-scope", "-DADDRESS_SANITIZER", "-g", "-fno-omit-frame-pointer", "-O0"]
TEST_LINKOPTS = ["-fsanitize=address", "-static-libasan"]

cc_test(
    name = "rect-tests",
    timeout = "short",
    srcs = [
        "rect.cpp",
        "main.cpp",
    ],
    copts = TEST_COPTS,
    linkopts = TEST_LINKOPTS,
    deps = [
        "//lib/common:rect",
        "@googletest//:gtest",
    ]
)
