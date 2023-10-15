load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="named",
  timeout = "short",
  srcs=["named.cpp"],
  deps=[
    "//lib/serialization/archive:binary_archive",
    "//lib/serialization/stream:file_stream", 
    "//lib/serialization/primitives",
  ],
  visibility=["//visibility:public"],
)
