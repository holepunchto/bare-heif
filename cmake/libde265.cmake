include_guard()

include(ExternalProject)

set(de265_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/libde265")

ExternalProject_Add(
  de265_git
  GIT_REPOSITORY "https://github.com/strukturag/libde265.git"
  GIT_TAG v1.0.15
  PREFIX "${de265_PREFIX}"
  BUILD_BYPRODUCTS "${de265_PREFIX}/lib/libde265.a"
  CMAKE_ARGS
    "-DBUILD_SHARED_LIBS=OFF"
    "-DCMAKE_INSTALL_PREFIX=${de265_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_MESSAGE_LOG_LEVEL=${CMAKE_MESSAGE_LOG_LEVEL}"
    "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
)

add_library(de265 STATIC IMPORTED GLOBAL)

add_dependencies(de265 de265_git)

set_target_properties(
  de265
  PROPERTIES
  IMPORTED_LOCATION "${de265_PREFIX}/lib/libde265.a"
)

file(MAKE_DIRECTORY "${de265_PREFIX}/include")

target_include_directories(
  de265
  INTERFACE "${de265_PREFIX}/include"
)
