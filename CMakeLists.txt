cmake_minimum_required(VERSION 3.31)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

find_package(cmake-bare REQUIRED PATHS node_modules/cmake-bare)
find_package(cmake-ports REQUIRED PATHS node_modules/cmake-ports)

project(bare_heif C CXX)

find_port(
  libheif
  FEATURES
    dav1d
    de265
)

add_bare_module(bare_heif)

target_sources(
  ${bare_heif}
  PRIVATE
    binding.c
)

target_link_libraries(
  ${bare_heif}
  PUBLIC
    heif
)
