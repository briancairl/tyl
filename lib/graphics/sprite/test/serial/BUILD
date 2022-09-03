load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="sprite",
  timeout = "short",
  srcs=[
    "animation.cpp",
    "spritesheet.cpp",
  ],
  deps=[
    "//lib/graphics/sprite:sprite_serial",
    "//lib/serialization/archive:json_archive",
    "//lib/serialization/stream:mem_stream", 
  ],
  visibility=["//visibility:public"]
)
