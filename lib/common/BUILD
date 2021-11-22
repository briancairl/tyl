cc_library(
  name="assert",
  hdrs=["include/assert.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/common",
  linkopts=[],
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="array_view",
  hdrs=["include/array_view.hpp"] + glob(["include/array_view/*.hpp"]),
  strip_include_prefix="include",
  include_prefix="tyl/common",
  linkopts=[],
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="bitops",
  hdrs=["include/bitops.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/common",
  linkopts=[],
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="filesystem",
  hdrs=["include/filesystem.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/common",
  linkopts=["-lstdc++fs"],
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="matrix",
  hdrs=["include/matrix.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/common",
  linkopts=[],
  deps=["@eigen//:eigen"],
  visibility=["//visibility:public"]
)

cc_library(
  name="ecs",
  hdrs=["include/ecs.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/common",
  linkopts=[],
  deps=["@entt//:entt"],
  visibility=["//visibility:public"]
)

cc_library(
  name="macros",
  hdrs=["include/strong_alias.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/common",
  linkopts=[],
  deps=[],
  visibility=["//visibility:public"]
)
