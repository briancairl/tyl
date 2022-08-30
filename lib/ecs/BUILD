cc_library(
  name="ecs",
  hdrs=[
    "include/ecs.hpp",
  ],
  strip_include_prefix="include",
  include_prefix="tyl/ecs",
  deps = [
    "@entt//:entt",
  ],
  visibility=["//visibility:public"]
)

cc_library(
  name="ecs_serial",
  hdrs=[
    "serial/reader.hpp",
    "serial/writer.hpp",
  ],
  strip_include_prefix="serial",
  include_prefix="tyl/serial/ecs",
  deps = [
    ":ecs",
    "//lib/common/reflection:typestr",
    "//lib/serialization/archive",
    "//lib/serialization/primitives",
  ],
  visibility=["//visibility:public"]
)
