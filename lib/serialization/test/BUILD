load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="binary_archive",
  timeout = "short",
  srcs=["binary_archive.cpp"],
  deps=["//lib/serialization:binary_archive", "//lib/serialization:file_stream",],
  visibility=["//visibility:public"],
)

gtest(
  name="file_stream",
  timeout = "short",
  srcs=["file_stream.cpp"],
  deps=["//lib/serialization:file_stream",],
  visibility=["//visibility:public"],
  data=["resources/file_stream.dat"]
)
