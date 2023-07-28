cc_library(
  name="asset_manager",
  hdrs=["include/asset_manager.hpp"],
  srcs=["src/asset_manager.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/engine/widgets",
  deps=["@entt", "@imgui", "@imgui-file-dialogue", "//lib/common/utility:expected", "//lib/engine/core:resource", "//lib/engine/core:drag_and_drop", "//lib/graphics/device"],
  visibility=["//visibility:public"]
)

cc_library(
  name="tileset_creator",
  hdrs=["include/tileset_creator.hpp"],
  srcs=["src/tileset_creator.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/engine/widgets",
  deps=["@entt", "@imgui", "//lib/common/utility:expected", "//lib/engine/core:resource", "//lib/graphics/device"],
  visibility=["//visibility:public"]
)
