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
  name="filesystem",
  hdrs=["include/filesystem.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl",
  linkopts=["-lstdc++fs"],
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
  name="serialization",
  hdrs=glob(["include/serialization/**/**"]) + ["include/serialization.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl",
  linkopts=[],
  deps=["@boost//:serialization"],
  visibility=["//visibility:public"]
)

cc_library(
  name="ecs",
  hdrs=glob(["include/ecs/**/**"]) + ["include/ecs.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl",
  linkopts=[],
  deps=["@entt//:entt", ":serialization"],
  visibility=["//visibility:public"]
)

cc_library(
  name="components",
  hdrs=["include/components.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl",
  linkopts=[],
  deps=[":vec", ":strong_alias", ":rect"],
  visibility=["//visibility:public"]
)

cc_library(
  name="time",
  hdrs=["include/time.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl",
  linkopts=[],
  deps=[],
  visibility=["//visibility:public"]
)
