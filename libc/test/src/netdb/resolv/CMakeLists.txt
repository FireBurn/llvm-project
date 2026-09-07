add_libc_testsuite(libc-netdb-resolv-tests)

add_libc_test(
  dns_message_test
  SUITE
    libc-netdb-resolv-tests
  SRCS
    dns_message_test.cpp
  DEPENDS
    libc.src.netdb.resolv.dns_message
    libc.src.string.strcmp
)

add_libc_test(
  resolv_conf_test
  SUITE
    libc-netdb-resolv-tests
  SRCS
    resolv_conf_test.cpp
  DEPENDS
    libc.hdr.sys_socket_macros
    libc.src.__support.CPP.string_view
    libc.src.netdb.resolv.resolv_conf
    libc.src.string.strcmp
)
