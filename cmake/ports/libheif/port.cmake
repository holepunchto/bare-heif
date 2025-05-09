include_guard(GLOBAL)

include(CheckCXXSymbolExists)

check_cxx_symbol_exists(_LIBCPP_VERSION cstdlib HAVE_LIBCPP)

if(HAVE_LIBCPP)
  set(libc++ c++)
else()
  set(libc++ stdc++)
endif()

add_library(heif STATIC IMPORTED GLOBAL)

set(args
  -DBUILD_SHARED_LIBS=OFF
  -DBUILD_TESTING=OFF
  -DENABLE_PLUGIN_LOADING=OFF
  -DWITH_EXAMPLES=OFF
  -DWITH_LIBSHARPYUV=OFF
  -DWITH_GDK_PIXBUF=OFF
  -DWITH_X265=OFF
  -DWITH_OpenH264_ENCODER=OFF
  -DWITH_OpenH264_DECODER=OFF
  -DWITH_AOM_ENCODER=OFF
  -DWITH_AOM_DECODER=OFF
)

set(depends)

if("dav1d" IN_LIST features)
  find_port(dav1d)

  list(APPEND depends dav1d)

  list(APPEND args
    -DWITH_DAV1D=ON
    "-DDAV1D_INCLUDE_DIR=$<TARGET_PROPERTY:dav1d,INTERFACE_INCLUDE_DIRECTORIES>"
    "-DDAV1D_LIBRARY=$<TARGET_FILE:dav1d>"
  )

  target_link_libraries(heif INTERFACE dav1d)
else()
  list(APPEND args -DWITH_DAVID=OFF)
endif()

if("de265" IN_LIST features)
  find_port(libde265)

  list(APPEND depends de265)

  list(APPEND args
    -DWITH_LIBDE265=ON
    "-DLIBDE265_INCLUDE_DIR=$<TARGET_PROPERTY:de265,INTERFACE_INCLUDE_DIRECTORIES>"
    "-DLIBDE265_LIBRARY=$<TARGET_FILE:de265>"
  )

  target_link_libraries(heif INTERFACE de265)
else()
  list(APPEND args -DWITH_LIBDE265=OFF)
endif()

if("jpeg" IN_LIST features)
  find_port(libjpeg)

  list(APPEND depends jpeg)

  list(APPEND args
    -DWITH_JPEG_DECODER=ON
    -DWITH_JPEG_ENCODER=ON
    "-DJPEG_INCLUDE_DIR=$<TARGET_PROPERTY:jpeg,INTERFACE_INCLUDE_DIRECTORIES>"
    "-DJPEG_LIBRARY=$<TARGET_FILE:jpeg>"
  )

  target_link_libraries(heif INTERFACE jpeg)
else()
  list(APPEND args
    -DWITH_JPEG_DECODER=OFF
    -DWITH_JPEG_ENCODER=OFF
  )
endif()

if(WIN32)
  set(lib heif.lib)
else()
  set(lib libheif.a)
endif()

declare_port(
  "github:strukturag/libheif@1.19.5"
  heif
  BYPRODUCTS lib/${lib}
  PATCHES
    patches/01-windows-warnings.patch
    patches/02-remove-doxygen.patch
    patches/03-static-build.patch
  DEPENDS ${depends}
  ARGS ${args}
)

add_dependencies(heif ${heif})

set_target_properties(
  heif
  PROPERTIES
  IMPORTED_LOCATION "${heif_PREFIX}/lib/${lib}"
)

file(MAKE_DIRECTORY "${heif_PREFIX}/include")

target_include_directories(
  heif
  INTERFACE "${heif_PREFIX}/include"
)

target_compile_definitions(
  heif
  INTERFACE
    LIBHEIF_STATIC_BUILD=1
    LIBHEIF_EXPORTS=0
)

target_link_libraries(
  heif
  INTERFACE
    ${libc++}
)
