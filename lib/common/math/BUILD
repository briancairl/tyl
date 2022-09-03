cc_library(
  name="geometry_core",
  hdrs=[
    "include/vec.hpp",
    "include/rect.hpp",
    "include/size.hpp",
  ],
  strip_include_prefix="include",
  include_prefix="tyl/math",
  deps=["@eigen"],
  visibility=["//visibility:public"]
)

cc_library(
  name="geometry_serial",
  hdrs=[
    "serial/vec.hpp",
    "serial/rect.hpp",
    "serial/size.hpp",
  ],
  strip_include_prefix="serial",
  include_prefix="tyl/serial/math",
  deps=[":geometry_core", "//lib/serialization:object",],
  visibility=["//visibility:public"]
)

cc_library(
  name="geometry",
  deps=[":geometry_core", ":geometry_serial"],
  visibility=["//visibility:public"]
)
