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
