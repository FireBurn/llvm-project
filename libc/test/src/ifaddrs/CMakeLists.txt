add_libc_testsuite(libc-ifaddrs-tests)

add_libc_test(
  getifaddrs_test
  SUITE
    libc-ifaddrs-tests
  SRCS
    getifaddrs_test.cpp
  DEPENDS
    libc.hdr.net_if_macros
    libc.hdr.sys_socket_macros
    libc.hdr.types.struct_ifaddrs
    libc.src.ifaddrs.freeifaddrs
    libc.src.ifaddrs.getifaddrs
    libc.src.string.strcmp
)
