include_guard()

include(ExternalProject)

set(aom_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/aom")

ExternalProject_Add(
  aom_git
  GIT_REPOSITORY "https://aomedia.googlesource.com/aom.git"
  GIT_TAG v3.11.0
  PREFIX "${aom_PREFIX}"
  BUILD_BYPRODUCTS "${aom_PREFIX}/lib/libaom.a"
  CMAKE_ARGS
    "-DBUILD_SHARED_LIBS=OFF"
    "-DCMAKE_INSTALL_PREFIX=${aom_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_MESSAGE_LOG_LEVEL=${CMAKE_MESSAGE_LOG_LEVEL}"
    "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
)

add_library(aom STATIC IMPORTED GLOBAL)

add_dependencies(aom aom_git)

set_target_properties(
  aom
  PROPERTIES
  IMPORTED_LOCATION "${aom_PREFIX}/lib/libaom.a"
)

file(MAKE_DIRECTORY "${aom_PREFIX}/include")

target_include_directories(
  aom
  INTERFACE "${aom_PREFIX}/include"
)
