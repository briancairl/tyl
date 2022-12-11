load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="kernel_op",
  timeout = "short",
  srcs=["kernel_op.cpp"],
  deps=[
    "@eigen",
    "//lib/common/grid:kernel_op",
  ],
  visibility=["//visibility:public"],
)
