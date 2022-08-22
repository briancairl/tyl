load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="serialization",
  timeout = "short",
  srcs=["serialization.cpp"],
  deps=["//lib/ecs:serialization", "//lib/serialization/archive:binary_archive", "//lib/serialization/archive:json_archive", "//lib/serialization/stream:file_stream",],
  visibility=["//visibility:public"],
)
