cc_binary(
  name="weather",
  srcs=["weather.cpp"],
  data=["heightmap_ds.png", "heightmap_ds_2.png"],
  deps=[
    "//systems/window:window",
    "//lib/common/math:geometry_core",
    "//lib/common/grid:kernel_op",
    "//lib/graphics/device",
    "//lib/graphics/device/glad:glad",
    "//lib/graphics/host:image",
  ]
)
