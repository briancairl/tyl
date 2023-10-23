load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="named",
  timeout = "short",
  srcs=["named.cpp"],
  deps=[
    "//core/serialization/archive:binary_archive",
    "//core/serialization/stream:file_stream", 
    "//core/serialization/primitives",
  ],
  visibility=["//visibility:public"],
)
