cmake_minimum_required(VERSION 3.25)

find_package(cmake-bare REQUIRED PATHS node_modules/cmake-bare)

project(bare_heif C)

include(cmake/aom.cmake)
include(cmake/libde265.cmake)
include(cmake/libheif.cmake)

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
