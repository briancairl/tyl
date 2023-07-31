load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="worker_pool",
  timeout = "short",
  srcs=["worker_pool.cpp"],
  deps=["//lib/common/async:worker_pool",],
  visibility=["//visibility:public"],
)
