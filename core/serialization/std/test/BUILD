load("@tyl//:bazel/test_rules.bzl", "gtest")

gtest(
  name="chrono",
  timeout = "short",
  srcs=["chrono.cpp"],
  deps=[
    "//core/serialization/archive:binary_archive",
    "//core/serialization/stream:mem_stream", 
    "//core/serialization/std",
  ],
  visibility=["//visibility:public"],
)

gtest(
  name="filesystem",
  timeout = "short",
  srcs=["filesystem.cpp"],
  deps=[
    "//core/serialization/archive:binary_archive",
    "//core/serialization/stream:mem_stream", 
    "//core/serialization/std",
  ],
  visibility=["//visibility:public"],
)

gtest(
  name="string",
  timeout = "short",
  srcs=["string.cpp"],
  deps=[
    "//core/serialization/archive:binary_archive",
    "//core/serialization/stream:mem_stream", 
    "//core/serialization/std",
  ],
  visibility=["//visibility:public"],
)

gtest(
  name="tuple",
  timeout = "short",
  srcs=["tuple.cpp"],
  deps=[
    "//core/serialization/archive:binary_archive",
    "//core/serialization/stream:mem_stream", 
    "//core/serialization/std",
  ],
  visibility=["//visibility:public"],
)

gtest(
  name="utility",
  timeout = "short",
  srcs=["utility.cpp"],
  deps=[
    "//core/serialization/archive:binary_archive",
    "//core/serialization/stream:mem_stream", 
    "//core/serialization/std",
  ],
  visibility=["//visibility:public"],
)

gtest(
  name="vector",
  timeout = "short",
  srcs=["vector.cpp"],
  deps=[
    "//core/serialization/archive:binary_archive",
    "//core/serialization/stream:mem_stream", 
    "//core/serialization/std",
  ],
  visibility=["//visibility:public"],
)
