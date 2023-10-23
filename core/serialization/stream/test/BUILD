load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="file_stream",
  timeout = "short",
  srcs=["file_stream.cpp"],
  deps=["//core/serialization/stream:file_stream",],
  visibility=["//visibility:public"],
  data=["resources/file_stream.dat"]
)

gtest(
  name="mem_stream",
  timeout = "short",
  srcs=["mem_stream.cpp"],
  deps=["//core/serialization/stream:mem_stream",],
  visibility=["//visibility:public"],
)
