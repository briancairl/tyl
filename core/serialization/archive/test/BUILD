load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="binary_archive",
  timeout = "short",
  srcs=["binary_archive.cpp"],
  deps=["//core/serialization/archive:binary_archive", "//core/serialization/stream:file_stream",],
  visibility=["//visibility:public"],
)

gtest(
  name="json_archive",
  timeout = "short",
  srcs=["json_archive.cpp"],
  deps=["//core/serialization/archive:json_archive", "//core/serialization/stream:file_stream", "//core/serialization/primitives", ],
  visibility=["//visibility:public"],
)
