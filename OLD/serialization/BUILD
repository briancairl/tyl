cc_library(
  name="object",
  hdrs=["object.hpp"],
  include_prefix="tyl/serialization",
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="serialization",
  hdrs=["serial.hpp"],
  include_prefix="tyl",
  deps=["//lib/serialization/primitives", ":object"],
  visibility=["//visibility:public"]
)
