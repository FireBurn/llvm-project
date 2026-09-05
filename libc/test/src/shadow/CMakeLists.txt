add_custom_target(libc_shadow_unittests)

if(NOT TARGET libc.src.shadow.shadow_utils)
  return()
endif()

add_libc_test(
  shadow_utils_test
  SUITE
    libc_shadow_unittests
  SRCS
    shadow_utils_test.cpp
  DEPENDS
    libc.hdr.types.struct_spwd
    libc.src.__support.CPP.span
    libc.src.shadow.shadow_utils
)
