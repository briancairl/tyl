TEST_COPTS = ["-Iexternal/googletest/googletest/include", "-fsanitize=address", "-fsanitize-address-use-after-scope", "-DADDRESS_SANITIZER", "-g", "-fno-omit-frame-pointer", "-O0"]
TEST_LINKOPTS = ["-fsanitize=address", "-static-libasan"]

cc_test(
    name = "wfc-tests",
    timeout = "short",
    srcs = [
        "adjacency_tests.cpp",
        "main.cpp",
        "wave_tests.cpp"
    ],
    copts = TEST_COPTS,
    linkopts = TEST_LINKOPTS,
    deps = [
        "//lib/wfc",
        "@googletest//:gtest",
    ]
)
