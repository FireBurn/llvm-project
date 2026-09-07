add_custom_target(libc_netinet_ether_unittests)

add_libc_test(
  ether_aton_test
  SUITE
    libc_netinet_ether_unittests
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
