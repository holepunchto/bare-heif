include_guard()

include(ExternalProject)

set(heif_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/libheif")

ExternalProject_Add(
  heif_git
  GIT_REPOSITORY "https://github.com/strukturag/libheif.git"
  GIT_TAG v1.19.5
  PREFIX "${heif_PREFIX}"
  BUILD_BYPRODUCTS "${heif_PREFIX}/lib/libheif.a"
  CMAKE_ARGS
    "-DBUILD_SHARED_LIBS=OFF"
    "-DCMAKE_INSTALL_PREFIX=${heif_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_MESSAGE_LOG_LEVEL=${CMAKE_MESSAGE_LOG_LEVEL}"
    "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"

    "-DWITH_LIBDE265=ON"
    "-DLIBDE265_INCLUDE_DIR=${de265_PREFIX}/include"
    "-DLIBDE265_LIBRARY=${de265_PREFIX}/lib/libde265.a"

    "-DWITH_AOM_DECODER=ON"
    "-DAOM_INCLUDE_DIR=${aom_PREFIX}/include"
    "-DAOM_LIBRARY=${aom_PREFIX}/lib/libaom.a"

    "-DWITH_X265=OFF"
    "-DWITH_OpenH264_ENCODER=OFF"
    "-DWITH_AOM_ENCODER=OFF"
  DEPENDS
    aom
    de265
)

add_library(heif STATIC IMPORTED GLOBAL)

add_dependencies(heif heif_git)

set_target_properties(
  heif
  PROPERTIES
  IMPORTED_LOCATION "${heif_PREFIX}/lib/libheif.a"
)

file(MAKE_DIRECTORY "${heif_PREFIX}/include")

target_include_directories(
  heif
  INTERFACE "${heif_PREFIX}/include"
)

target_link_libraries(
  heif
  INTERFACE
    aom
    de265
)
