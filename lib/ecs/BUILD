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
  name="serialization_impl",
  hdrs=[
    "src/load.hpp",
    "src/save.hpp",
  ],
  strip_include_prefix="src",
  include_prefix="tyl/ecs",
  deps = [
    ":ecs",
    "//lib/common:typestr",
    "//lib/serialization/archive",
    "//lib/serialization/types/primitives",
  ],
  visibility=["//visibility:private"]
)


cc_library(
  name="serialization",
  hdrs=["include/serialization.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/ecs",
  deps = [
    ":serialization_impl",
  ],
  visibility=["//visibility:public"]
)
