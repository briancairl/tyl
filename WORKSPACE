workspace(name="tyl")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")


# ImGui
http_archive(
  name="imgui",
  url="https://github.com/ocornut/imgui/archive/v1.80.zip",
  sha256="9da756d65d18a34cbb4f30ae64e6d6c42f94907b070ad3abaf553cb99e4ad403",
  build_file="@//external:imgui.BUILD",
  strip_prefix="imgui-1.80"
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
