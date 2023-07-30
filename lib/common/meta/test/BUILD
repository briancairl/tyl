load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="meta",
  timeout = "short",
  srcs=["meta.cpp"],
  deps=["//lib/common/meta",],
  visibility=["//visibility:public"],
)
