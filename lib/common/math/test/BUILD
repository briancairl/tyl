load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="geometry_serial",
  timeout = "short",
  srcs=["geometry.cpp"],
  deps=[
    "//lib/serialization/archive:binary_archive",
    "//lib/serialization/stream:mem_stream", 
    "//lib/common/math:geometry_serial",
  ],
  visibility=["//visibility:public"],
)
