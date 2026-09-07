add_custom_target(libc_resolv_unittests)

if(NOT TARGET libc.src.resolv.dn_expand)
  return()
endif()

add_libc_test(
  dn_expand_test
  SUITE
    libc_resolv_unittests
  SRCS
    dn_expand_test.cpp
  DEPENDS
    libc.src.resolv.dn_comp
    libc.src.resolv.dn_expand
    libc.src.string.memory_utils.inline_memcpy
)
