include_guard(GLOBAL)

include(CheckCXXSymbolExists)

check_cxx_symbol_exists(_LIBCPP_VERSION cstdlib HAVE_LIBCPP)

if(HAVE_LIBCPP)
  set(libc++ c++)
else()
  set(libc++ stdc++)
endif()

if(WIN32)
  set(lib libde265.lib)
else()
  set(lib libde265.a)
endif()

declare_port(
  "github:strukturag/libde265@1.0.15"
  de265
  BYPRODUCTS lib/${lib}
  PATCHES
    patches/01-windows-clang.patch
    patches/02-getopt-const-pointer.patch
    patches/03-clang-sse-flags.patch
    patches/04-msvc-runtime-policy.patch
  ARGS
    -DBUILD_SHARED_LIBS=OFF
    -DENABLE_SDL=OFF
    -DENABLE_DECODER=OFF
    -DENABLE_ENCODER=OFF
)

add_library(de265 STATIC IMPORTED GLOBAL)

add_dependencies(de265 ${de265})

set_target_properties(
  de265
  PROPERTIES
  IMPORTED_LOCATION "${de265_PREFIX}/lib/${lib}"
)

file(MAKE_DIRECTORY "${de265_PREFIX}/include")

target_include_directories(
  de265
  INTERFACE "${de265_PREFIX}/include"
)

target_compile_definitions(
  de265
  INTERFACE
    LIBDE265_STATIC_BUILD=1
    LIBDE265_EXPORTS=0
)

target_link_libraries(
  de265
  INTERFACE
    ${libc++}
)
