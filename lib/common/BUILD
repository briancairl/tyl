cc_library(
  name="assert",
  hdrs=["include/assert.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl",
  linkopts=[],
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="strong_alias",
  hdrs=["include/strong_alias.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl",
  linkopts=[],
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="array_view",
  hdrs=["include/array_view.hpp"] + glob(["include/array_view/*.hpp"]),
  strip_include_prefix="include",
  include_prefix="tyl",
  linkopts=[],
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="bitops",
  hdrs=["include/bitops.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl",
  linkopts=[],
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="types",
  hdrs=["include/strong_alias.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl",
  linkopts=[],
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="vec",
  hdrs=["include/vec.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl",
  linkopts=[],
  deps=["@eigen//:eigen"],
  visibility=["//visibility:public"]
)

cc_library(
  name="rect",
  hdrs=["include/rect.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl",
  linkopts=[],
  deps=[":vec"],
  visibility=["//visibility:public"]
)

cc_library(
  name="ecs",
  hdrs=["include/ecs.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl",
  linkopts=[],
  deps=["@entt//:entt"],
  visibility=["//visibility:public"]
)

cc_library(
  name="components",
  hdrs=["include/components.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl",
  linkopts=[],
  deps=[":vec", ":strong_alias"],
  visibility=["//visibility:public"]
)
