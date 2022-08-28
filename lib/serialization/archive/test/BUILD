load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="binary_archive",
  timeout = "short",
  srcs=["binary_archive.cpp"],
  deps=["//lib/serialization/archive:binary_archive", "//lib/serialization/stream:file_stream",],
  visibility=["//visibility:public"],
)

gtest(
  name="json_archive",
  timeout = "short",
  srcs=["json_archive.cpp"],
  deps=["//lib/serialization/archive:json_archive", "//lib/serialization/stream:file_stream", "//lib/serialization/primitives", "//lib/serialization/types/std",],
  visibility=["//visibility:public"],
)
