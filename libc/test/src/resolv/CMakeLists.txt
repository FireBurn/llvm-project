add_libc_testsuite(libc-resolv-tests)

if(NOT TARGET libc.src.resolv.dn_expand)
  return()
endif()

add_libc_test(
  dn_expand_test
  SUITE
    libc-resolv-tests
  SRCS
    dn_expand_test.cpp
  DEPENDS
    libc.src.resolv.dn_comp
    libc.src.resolv.dn_expand
    libc.src.string.memory_utils.inline_memcpy
)
