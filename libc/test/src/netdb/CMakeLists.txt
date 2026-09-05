add_subdirectory(resolv)

add_custom_target(libc_netdb_unittests)
add_custom_target(libc-netdb-tests)

add_libc_test(
  netdb_test
  SUITE
    libc-netdb-tests
  SRCS
    netdb_test.cpp
  DEPENDS
    libc.hdr.netdb_macros
    libc.hdr.sys_socket_macros
    libc.hdr.types.struct_addrinfo
    libc.hdr.types.struct_sockaddr_in
    libc.hdr.types.struct_sockaddr_in6
    libc.src.netdb.freeaddrinfo
    libc.src.netdb.getaddrinfo
    libc.test.UnitTest.ErrnoCheckingTest
)

add_libc_test(
  getnameinfo_test
  SUITE
    libc_netdb_unittests
  SRCS
    getnameinfo_test.cpp
  DEPENDS
    libc.hdr.netdb_macros
    libc.hdr.types.struct_sockaddr
    libc.hdr.types.struct_sockaddr_in
    libc.hdr.types.struct_sockaddr_in6
    libc.include.sys_socket
    libc.src.__support.CPP.string_view
    libc.src.arpa.inet.htons
    libc.src.netdb.getnameinfo
)
