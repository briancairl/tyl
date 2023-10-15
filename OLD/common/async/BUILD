cc_library(
  name="non_blocking_future",
  hdrs=["include/non_blocking_future.hpp",],
  strip_include_prefix="include",
  include_prefix="tyl/async",
  deps=["//lib/common/utility:expected"],
  visibility=["//visibility:public"]
)

cc_library(
  name="synchronized",
  hdrs=["include/synchronized.hpp",],
  strip_include_prefix="include",
  include_prefix="tyl/async",
  deps=[],
  visibility=["//visibility:public"]
)

cc_library(
  name="worker_pool",
  hdrs=["include/worker_pool.hpp",],
  strip_include_prefix="include",
  include_prefix="tyl/async",
  deps=[":non_blocking_future"],
  visibility=["//visibility:public"]
)
