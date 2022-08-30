load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="animation",
  timeout = "short",
  srcs=["animation.cpp"],
  deps=[
    "//lib/graphics/serialization/sprite",
    "//lib/serialization/archive:json_archive",
    "//lib/serialization/stream:mem_stream", 
  ],
  visibility=["//visibility:public"],
)

gtest(
  name="spritesheet",
  timeout = "short",
  srcs=["spritesheet.cpp"],
  deps=[
    "//lib/graphics/serialization/sprite",
    "//lib/serialization/archive:json_archive",
    "//lib/serialization/stream:mem_stream", 
  ],
  visibility=["//visibility:public"],
)
