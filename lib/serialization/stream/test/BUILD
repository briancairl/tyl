load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="file_stream",
  timeout = "short",
  srcs=["file_stream.cpp"],
  deps=["//lib/serialization/stream:file_stream",],
  visibility=["//visibility:public"],
  data=["resources/file_stream.dat"]
)
