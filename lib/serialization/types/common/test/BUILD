load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="math",
  timeout = "short",
  srcs=["math.cpp"],
  deps=[
    "//lib/serialization/archive:binary_archive",
    "//lib/serialization/stream:mem_stream", 
    "//lib/serialization/types/common:math",
  ],
  visibility=["//visibility:public"],
)
