cc_library(
  name="object",
  hdrs=["object.hpp"],
  include_prefix="tyl/serialization",
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="serialization",
  hdrs=["serialization.hpp"],
  include_prefix="tyl",
  deps=["//core/serialization/primitives", ":object"],
  visibility=["//visibility:public"]
)
