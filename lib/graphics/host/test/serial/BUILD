load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="image",
  timeout = "short",
  srcs=["image.cpp"],
  deps=[
    "//lib/graphics/host:image_serial",
    "//lib/serialization/archive:json_archive",
    "//lib/serialization/stream:mem_stream", 
  ],
  visibility=["//visibility:public"],
)