workspace(name="tyl")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Boost
git_repository(
    name="com_github_nelhage_rules_boost",
    commit="d104cb7beba996d67ae5826be07aab2d9ca0ee38",
    remote="https://github.com/nelhage/rules_boost",
    shallow_since="1637888414 -0800",
)
load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

# Entt
git_repository(
  name="entt",
  remote="https://github.com/skypjack/entt.git",
  commit="7a949dd32849e8230b15a5ff8ad1104c7f748c2a",
  shallow_since="1610207127 +0100"
)

# MultiFieldArray
git_repository(
  name="multi_field_array",
  remote="git@github.com:briancairl/multi_field_array.git",
  commit="f65a42d8eb9f0ee5bc691b994e36f49de338a2d7",
  shallow_since="1633584653 -0400"
)

# Eigen
http_archive(
    name="eigen",
    url="https://github.com/eigenteam/eigen-git-mirror/archive/3.3.4.zip",
    sha256="f5580adc34ea45a4c30200e4100f8a55c55af22b77d4ed05985118fd0b15b77e",
    build_file="eigen.BUILD",
    strip_prefix="eigen-git-mirror-3.3.4",
)

# ImGui
http_archive(
  name="imgui",
  url="https://github.com/ocornut/imgui/archive/v1.80.zip",
  sha256="9da756d65d18a34cbb4f30ae64e6d6c42f94907b070ad3abaf553cb99e4ad403",
  build_file="@//external:imgui.BUILD",
  strip_prefix="imgui-1.80"
)

# OpenAL
new_local_repository(
  name="openal",
  path="/usr/",
  build_file="@//external:openal.BUILD",
)

# GLFW
new_local_repository(
  name="glfw",
  path="/usr/",
  build_file="@//external:glfw.BUILD",
)

# STB
new_git_repository(
  name="stb",
  remote="https://github.com/nothings/stb.git",
  commit="b42009b3b9d4ca35bc703f5310eedc74f584be58",
  shallow_since="1594640766 -0700",
  build_file="@//external:stb.BUILD",
)

# SPDLOG
new_git_repository(
  name="spdlog",
  remote="git@github.com:gabime/spdlog.git",
  commit="5b4c4f3f770acbd25400d866f3fc2fdf669d5b7e",
  shallow_since="1627377994 +0300",
  build_file="@//external:spdlog.BUILD",
)

# GTest/GMock
http_archive(
    name="googletest",
    url="https://github.com/google/googletest/archive/release-1.8.0.zip",
    sha256="f3ed3b58511efd272eb074a3a6d6fb79d7c2e6a0e374323d1e6bcbcc1ef141bf",
    strip_prefix="googletest-release-1.8.0",
    build_file="@//external:googletest.BUILD",
)
