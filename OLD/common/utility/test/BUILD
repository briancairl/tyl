load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="expected",
  timeout = "short",
  srcs=["expected.cpp"],
  deps=["//lib/common/utility:expected",],
  visibility=["//visibility:public"],
)
