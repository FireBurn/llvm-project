add_custom_target(libc_netdb_resolv_unittests)

add_libc_test(
  dns_message_test
  SUITE
    libc_netdb_resolv_unittests
  SRCS
    dns_message_test.cpp
  DEPENDS
    libc.src.netdb.resolv.dns_message
    libc.src.string.strcmp
)
