cc_library(
  name="object",
  hdrs=["object.hpp", "object_fwd.hpp"],
  include_prefix="tyl/serialization",
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="serialization",
  hdrs=["serialization.hpp", "serialization_fwd.hpp"],
  include_prefix="tyl",
  deps=[
    "//core/serialization/archive",
    "//core/serialization/stream",
    "//core/serialization/primitives",
    ":object"],
  visibility=["//visibility:public"]
)
