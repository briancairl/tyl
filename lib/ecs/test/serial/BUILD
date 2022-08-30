load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="ecs",
  timeout = "short",
  srcs=["ecs.cpp"],
  deps=["//lib/ecs:ecs_serial", "//lib/serialization/archive:binary_archive", "//lib/serialization/archive:json_archive", "//lib/serialization/stream:file_stream",],
  visibility=["//visibility:public"],
)
