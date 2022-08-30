load("@tyl//:bazel/test_rules.bzl", "gtest")

cc_library(
  name="test_types",
  hdrs=["test_types.hpp"],
  deps=["//lib/serialization/archive",],
  visibility=["//visibility:private"]
)

gtest(
  name="array",
  timeout = "short",
  srcs=["array.cpp"],
  deps=[
    ":test_types",
    "//lib/serialization/archive:json_archive",
    "//lib/serialization/stream:mem_stream", 
    "//lib/common/serialization/std",
  ],
  visibility=["//visibility:public"],
)

gtest(
  name="optional",
  timeout = "short",
  srcs=["optional.cpp"],
  deps=[
    ":test_types",
    "//lib/serialization/archive:json_archive",
    "//lib/serialization/stream:mem_stream", 
    "//lib/common/serialization/std",
  ],
  visibility=["//visibility:public"],
)

gtest(
  name="string",
  timeout = "short",
  srcs=["string.cpp"],
  deps=[
    "//lib/serialization/archive:json_archive",
    "//lib/serialization/stream:mem_stream", 
    "//lib/common/serialization/std",
  ],
  visibility=["//visibility:public"],
)


gtest(
  name="vector",
  timeout = "short",
  srcs=["vector.cpp"],
  deps=[
    ":test_types",
    "//lib/serialization/archive:json_archive",
    "//lib/serialization/stream:mem_stream", 
    "//lib/common/serialization/std",
  ],
  visibility=["//visibility:public"],
)

gtest(
  name="unordered_map",
  timeout = "short",
  srcs=["unordered_map.cpp"],
  deps=[
    ":test_types",
    "//lib/serialization/archive:json_archive",
    "//lib/serialization/stream:mem_stream", 
    "//lib/common/serialization/std",
  ],
  visibility=["//visibility:public"],
)
