### Audio declarations ###

cc_library(
  name="audio_hdrs",
  hdrs=[
    "include/device.hpp",
    "include/listener.hpp",
    "include/source.hpp",
    "include/sound.hpp",
    "include/fwd.hpp",
  ],
  strip_include_prefix="include",
  include_prefix="tyl/audio/device",
  visibility=["//visibility:private"]
)

### OpenAL implemenatation ###

cc_library(
  name="audio_impl__openal_user_types",
  hdrs=["include/typedef.hpp"],
  strip_include_prefix="include",
  include_prefix="tyl/audio/device",
  visibility=["//visibility:private"]
)

cc_library(
  name="audio_impl__openal_internal",
  hdrs=["src/openal/al.inl"],
  strip_include_prefix="src/openal",
  include_prefix="tyl/audio/device",
  deps=[
    "//core/common",
    ":audio_impl__openal_user_types",
  ],
  visibility=["//visibility:private"]
)

cc_library(
  name="audio_impl__openal",
  srcs=[
    "src/openal/device.cpp",
    "src/openal/listener.cpp",
    "src/openal/source.cpp",
    "src/openal/sound.cpp",
  ],
  deps=[
    "@openal//:openal",
    "//core/common",
    ":audio_hdrs",
    ":audio_impl__openal_internal"
  ],
  visibility=["//visibility:private"]
)

### Audio implemenatation ###

alias(
  name="device",
  actual=":audio_impl__openal",
  visibility=["//visibility:public"]
)
