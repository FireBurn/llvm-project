add_libc_testsuite(libc-resolv-tests)

if(NOT TARGET libc.src.resolv.dn_expand)
  return()
endif()

add_libc_test(
  res_n_test
  SUITE
    libc-resolv-tests
  SRCS
    res_n_test.cpp
  DEPENDS
    libc.hdr.types.struct___res_state
    libc.src.netdb.resolv.dns_message
    libc.src.resolv.res_nclose
    libc.src.resolv.res_ninit
    libc.src.resolv.res_nmkquery
    libc.src.resolv.res_nquery
    libc.src.string.memset
)

add_libc_test(
  b64_test
  SUITE
    libc-resolv-tests
  SRCS
    b64_test.cpp
  DEPENDS
    libc.src.resolv.b64_ntop
    libc.src.resolv.b64_pton
    libc.src.string.memory_utils.inline_memcmp
    libc.src.string.strlen
)

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

add_libc_test(
  res_query_test
  SUITE
    libc-resolv-tests
  SRCS
    res_query_test.cpp
  DEPENDS
    libc.hdr.types.struct___res_state
    libc.src.netdb.resolv.dns_message
    libc.src.resolv.__res_state
    libc.src.resolv.res_close
    libc.src.resolv.res_init
    libc.src.resolv.res_mkquery
    libc.src.resolv.res_query
    libc.src.resolv.res_search
    libc.src.resolv.res_send
)

add_libc_test(
  res_init_test
  SUITE
    libc-resolv-tests
  SRCS
    res_init_test.cpp
  DEPENDS
    libc.src.resolv.res_init
)
