load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="alias",
  timeout = "short",
  srcs=["alias.cpp"],
  deps=["//lib/common/utility:alias",],
  visibility=["//visibility:public"],
)
