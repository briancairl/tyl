cc_library(
  name="texture_asset_manager",
  hdrs=["include/texture_asset_manager.hpp"],
  srcs=["src/texture_asset_manager.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/engine/widgets",
  deps=["@imgui", "@imgui-file-dialogue", "//lib/common/utility:expected", "//lib/engine/core:asset", "//lib/engine/core:resources", "//lib/graphics/device", "//lib/common/utility:entt"],
  visibility=["//visibility:public"]
)

cc_library(
  name="text_asset_manager",
  hdrs=["include/text_asset_manager.hpp"],
  srcs=["src/text_asset_manager.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/engine/widgets",
  deps=["@imgui", "@imgui-file-dialogue", "//lib/common/utility:expected", "//lib/engine/core:asset", "//lib/engine/core:resources", "//lib/common/utility:entt"],
  visibility=["//visibility:public"]
)

cc_library(
  name="drag_and_drop",
  hdrs=["include/drag_and_drop.hpp"],
  srcs=["src/drag_and_drop.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/engine/widgets",
  deps=["@imgui", "//lib/common/utility:expected", "//lib/engine/core:asset", "//lib/engine/core:resources", "//lib/engine/core:drag_and_drop", "//lib/common/utility:entt"],
  visibility=["//visibility:public"]
)

cc_library(
  name="tileset_creator",
  hdrs=["include/tileset_creator.hpp"],
  srcs=["src/tileset_creator.cpp"],
  strip_include_prefix="include",
  include_prefix="tyl/engine/widgets",
  deps=["@imgui", "@imgui-file-dialogue", "//lib/common/utility:expected", "//lib/engine/core:asset", "//lib/engine/graphics:tileset", "//lib/engine/core:resources", "//lib/graphics/device", "//lib/common/utility:entt", "//lib/common/utility:dynamic_bitset"],
  visibility=["//visibility:public"]
)
