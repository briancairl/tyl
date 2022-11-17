load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="alias",
  timeout = "short",
  srcs=["alias.cpp"],
  deps=["//lib/common/utility:alias",
        "//lib/common/utility:alias_serial",
        "//lib/serialization/archive:binary_archive",
        "//lib/serialization/stream:mem_stream", ],
  visibility=["//visibility:public"],
)

gtest(
  name="bitfield",
  timeout = "short",
  srcs=["bitfield.cpp"],
  deps=["//lib/common/utility:bitfield"],
  visibility=["//visibility:public"],
)
