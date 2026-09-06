add_custom_target(libc_ifaddrs_unittests)

add_libc_test(
  getifaddrs_test
  SUITE
    libc_ifaddrs_unittests
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
