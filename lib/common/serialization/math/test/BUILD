load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="geometry",
  timeout = "short",
  srcs=["geometry.cpp"],
  deps=[
    "//lib/serialization/archive:binary_archive",
    "//lib/serialization/stream:mem_stream", 
    "//lib/common/serialization/math:geometry",
  ],
  visibility=["//visibility:public"],
)
