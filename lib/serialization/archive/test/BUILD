load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="binary_archive",
  timeout = "short",
  srcs=["binary_archive.cpp"],
  deps=["//lib/serialization/archive:binary_archive", "//lib/serialization/stream:file_stream",],
  visibility=["//visibility:public"],
)
