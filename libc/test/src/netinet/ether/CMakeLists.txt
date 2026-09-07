add_libc_testsuite(libc-netinet-ether-tests PARENT libc-netinet-tests)

add_libc_test(
  ether_ntoa_test
  SUITE
    libc-netinet-ether-tests
  SRCS
    ether_ntoa_test.cpp
  DEPENDS
    libc.hdr.types.struct_ether_addr
    libc.src.netinet.ether.ether_ntoa
)

add_libc_test(
  ether_ntoa_r_test
  SUITE
    libc-netinet-ether-tests
  SRCS
    ether_ntoa_r_test.cpp
  DEPENDS
    libc.hdr.types.struct_ether_addr
    libc.src.netinet.ether.ether_ntoa_r
)

add_libc_test(
  ether_aton_test
  SUITE
    libc-netinet-ether-tests
  SRCS
    ether_aton_test.cpp
  DEPENDS
    libc.src.netinet.ether.ether_aton
    libc.src.netinet.ether.ether_aton_r
    libc.src.netinet.ether.ether_line
    libc.src.netinet.ether.ether_ntoa
    libc.src.netinet.ether.ether_ntoa_r
    libc.src.string.strcmp
)

add_libc_test(
  ether_hostton_test
  SUITE
    libc-netinet-ether-tests
  SRCS
    ether_hostton_test.cpp
  DEPENDS
    libc.hdr.fcntl_macros
    libc.hdr.sys_stat_macros
    libc.hdr.types.struct_ether_addr
    libc.src.__support.CPP.string_view
    libc.src.fcntl.open
    libc.src.netinet.ether.ether_hostton
    libc.src.netinet.ether.ether_ntohost
    libc.src.netinet.ether.ethers
    libc.src.stdio.remove
    libc.src.unistd.close
    libc.src.unistd.write
)
