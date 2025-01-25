cmake_minimum_required(VERSION 3.25)

find_package(cmake-bare REQUIRED PATHS node_modules/cmake-bare)
find_package(cmake-ports REQUIRED PATHS node_modules/cmake-ports)

project(bare_heif C)

find_port(libheif)

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
