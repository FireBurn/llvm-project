//===-- Definition of macros from netinet/in.h ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_NETINET_IN_MACROS_H
#define LLVM_LIBC_MACROS_NETINET_IN_MACROS_H

#include "uapi-compat-macros.h"

#include "../__llvm-libc-common.h"
#include "../llvm-libc-types/in_addr_t.h"
#include "../llvm-libc-types/struct_in6_addr.h"

// The protocol numbers are the kernel's too, in <linux/in.h>. Leaving them
// to it is right only where that header has been read.
#if !(defined(__UAPI_DEF_IN_IPPROTO) && __UAPI_DEF_IN_IPPROTO &&               \
      defined(_LINUX_IN_H))
#define IPPROTO_IP 0
#define IPPROTO_ICMP 1
#define IPPROTO_IGMP 2
#define IPPROTO_IPIP 4
#define IPPROTO_TCP 6
#define IPPROTO_EGP 8
#define IPPROTO_PUP 12
#define IPPROTO_UDP 17
#define IPPROTO_IDP 22
#define IPPROTO_TP 29
#define IPPROTO_DCCP 33
#define IPPROTO_IPV6 41
#define IPPROTO_RSVP 46
#define IPPROTO_GRE 47
#define IPPROTO_ESP 50
#define IPPROTO_AH 51
#define IPPROTO_MTP 92
#define IPPROTO_BEETPH 94
#define IPPROTO_ENCAP 98
#define IPPROTO_PIM 103
#define IPPROTO_COMP 108
#define IPPROTO_L2TP 115
#define IPPROTO_SCTP 132
#define IPPROTO_UDPLITE 136
#define IPPROTO_MPLS 137
#define IPPROTO_ETHERNET 143
#define IPPROTO_AGGFRAG 144
#define IPPROTO_RAW 255
#define IPPROTO_SMC 256
#define IPPROTO_MPTCP 262

// Say the definitions here are the ones, so <linux/in.h> skips its own if
// it is read after this.
#undef __UAPI_DEF_IN_IPPROTO
#define __UAPI_DEF_IN_IPPROTO 0
#endif

// The extension headers an IPv6 packet may carry, which sit in the same
// number space as the protocols above. The kernel has these in
// <linux/in6.h>.
#if !(defined(__UAPI_DEF_IPPROTO_V6) && __UAPI_DEF_IPPROTO_V6 &&               \
      defined(_LINUX_IN6_H))
#define IPPROTO_HOPOPTS 0
#define IPPROTO_ROUTING 43
#define IPPROTO_FRAGMENT 44
#define IPPROTO_ICMPV6 58
#define IPPROTO_NONE 59
#define IPPROTO_DSTOPTS 60
#define IPPROTO_MH 135

#undef __UAPI_DEF_IPPROTO_V6
#define __UAPI_DEF_IPPROTO_V6 0
#endif

#define INADDR_ANY __LLVM_LIBC_CAST(static_cast, in_addr_t, 0x00000000)
#define INADDR_BROADCAST __LLVM_LIBC_CAST(static_cast, in_addr_t, 0xffffffff)
#define INADDR_NONE __LLVM_LIBC_CAST(static_cast, in_addr_t, 0xffffffff)
// Not specified by POSIX, added in SVR4
#define INADDR_LOOPBACK __LLVM_LIBC_CAST(static_cast, in_addr_t, 0x7f000001)

#define IN6ADDR_ANY_INIT                                                       \
  {                                                                            \
    {                                                                          \
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }                       \
    }                                                                          \
  }
#define IN6ADDR_LOOPBACK_INIT                                                  \
  {                                                                            \
    {                                                                          \
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }                       \
    }                                                                          \
  }

// The following macros test for special IPv6 addresses. Each macro is of type
// int and takes a single argument of type const struct in6_addr *:
// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/netinet_in.h.html

#define __IN6_IS_ADDR_UNSPECIFIED(a)                                           \
  ((a)->s6_addr32[0] == 0 && (a)->s6_addr32[1] == 0 &&                         \
   (a)->s6_addr32[2] == 0 && (a)->s6_addr32[3] == 0)

#define __IN6_IS_ADDR_LOOPBACK(a)                                              \
  ((a)->s6_addr32[0] == 0 && (a)->s6_addr32[1] == 0 &&                         \
   (a)->s6_addr32[2] == 0 && (a)->s6_addr[12] == 0 && (a)->s6_addr[13] == 0 && \
   (a)->s6_addr[14] == 0 && (a)->s6_addr[15] == 1)

#define IN6_IS_ADDR_UNSPECIFIED(a)                                             \
  (__extension__({                                                             \
    const struct in6_addr *__a = (a);                                          \
    __IN6_IS_ADDR_UNSPECIFIED(__a);                                            \
  }))

#define IN6_IS_ADDR_LOOPBACK(a)                                                \
  (__extension__({                                                             \
    const struct in6_addr *__a = (a);                                          \
    __IN6_IS_ADDR_LOOPBACK(__a);                                               \
  }))

#define IN6_IS_ADDR_MULTICAST(a) ((a)->s6_addr[0] == 0xff)

#define IN6_IS_ADDR_LINKLOCAL(a)                                               \
  (__extension__({                                                             \
    const struct in6_addr *__a = (a);                                          \
    __a->s6_addr[0] == 0xfe && (__a->s6_addr[1] & 0xc0) == 0x80;               \
  }))

#define IN6_IS_ADDR_SITELOCAL(a)                                               \
  (__extension__({                                                             \
    const struct in6_addr *__a = (a);                                          \
    __a->s6_addr[0] == 0xfe && (__a->s6_addr[1] & 0xc0) == 0xc0;               \
  }))

#define IN6_IS_ADDR_V4MAPPED(a)                                                \
  (__extension__({                                                             \
    const struct in6_addr *__a = (a);                                          \
    __a->s6_addr32[0] == 0 && __a->s6_addr32[1] == 0 &&                        \
        __a->s6_addr[8] == 0 && __a->s6_addr[9] == 0 &&                        \
        __a->s6_addr[10] == 0xff && __a->s6_addr[11] == 0xff;                  \
  }))

#define IN6_IS_ADDR_V4COMPAT(a)                                                \
  (__extension__({                                                             \
    const struct in6_addr *__a = (a);                                          \
    __a->s6_addr32[0] == 0 && __a->s6_addr32[1] == 0 &&                        \
        __a->s6_addr32[2] == 0 && !__IN6_IS_ADDR_UNSPECIFIED(__a) &&           \
        !__IN6_IS_ADDR_LOOPBACK(__a);                                          \
  }))

#define IN6_IS_ADDR_MC_NODELOCAL(a)                                            \
  (__extension__({                                                             \
    const struct in6_addr *__a = (a);                                          \
    IN6_IS_ADDR_MULTICAST(__a) && (__a->s6_addr[1] & 0xf) == 0x1;              \
  }))

#define IN6_IS_ADDR_MC_LINKLOCAL(a)                                            \
  (__extension__({                                                             \
    const struct in6_addr *__a = (a);                                          \
    IN6_IS_ADDR_MULTICAST(__a) && (__a->s6_addr[1] & 0xf) == 0x2;              \
  }))

#define IN6_IS_ADDR_MC_SITELOCAL(a)                                            \
  (__extension__({                                                             \
    const struct in6_addr *__a = (a);                                          \
    IN6_IS_ADDR_MULTICAST(__a) && (__a->s6_addr[1] & 0xf) == 0x5;              \
  }))

#define IN6_IS_ADDR_MC_ORGLOCAL(a)                                             \
  (__extension__({                                                             \
    const struct in6_addr *__a = (a);                                          \
    IN6_IS_ADDR_MULTICAST(__a) && (__a->s6_addr[1] & 0xf) == 0x8;              \
  }))

#define IN6_IS_ADDR_MC_GLOBAL(a)                                               \
  (__extension__({                                                             \
    const struct in6_addr *__a = (a);                                          \
    IN6_IS_ADDR_MULTICAST(__a) && (__a->s6_addr[1] & 0xf) == 0xe;              \
  }))

// Socket options at the IPPROTO_IP level.
#define IP_TOS 1
#define IP_TTL 2
#define IP_HDRINCL 3
#define IP_OPTIONS 4
#define IP_ROUTER_ALERT 5
#define IP_RECVOPTS 6
#define IP_RETOPTS 7
#define IP_PKTINFO 8
#define IP_PKTOPTIONS 9
#define IP_MTU_DISCOVER 10
#define IP_RECVERR 11
#define IP_RECVTTL 12
#define IP_RECVTOS 13
#define IP_MTU 14
#define IP_FREEBIND 15
#define IP_IPSEC_POLICY 16
#define IP_XFRM_POLICY 17
#define IP_PASSSEC 18
#define IP_TRANSPARENT 19
#define IP_ORIGDSTADDR 20
#define IP_RECVORIGDSTADDR IP_ORIGDSTADDR
#define IP_MINTTL 21
#define IP_NODEFRAG 22
#define IP_CHECKSUM 23
#define IP_BIND_ADDRESS_NO_PORT 24
#define IP_RECVFRAGSIZE 25

#define IP_MULTICAST_IF 32
#define IP_MULTICAST_TTL 33
#define IP_MULTICAST_LOOP 34
#define IP_ADD_MEMBERSHIP 35
#define IP_DROP_MEMBERSHIP 36
#define IP_UNBLOCK_SOURCE 37
#define IP_BLOCK_SOURCE 38
#define IP_ADD_SOURCE_MEMBERSHIP 39
#define IP_DROP_SOURCE_MEMBERSHIP 40
#define IP_MSFILTER 41
#define IP_MULTICAST_ALL 49

// What IP_MTU_DISCOVER may be set to.
#define IP_PMTUDISC_DONT 0
#define IP_PMTUDISC_WANT 1
#define IP_PMTUDISC_DO 2
#define IP_PMTUDISC_PROBE 3
#define IP_PMTUDISC_INTERFACE 4
#define IP_PMTUDISC_OMIT 5

// Socket options at the IPPROTO_IPV6 level.
#define IPV6_ADDRFORM 1
#define IPV6_2292PKTINFO 2
#define IPV6_2292HOPOPTS 3
#define IPV6_2292DSTOPTS 4
#define IPV6_2292RTHDR 5
#define IPV6_2292PKTOPTIONS 6
#define IPV6_CHECKSUM 7
#define IPV6_2292HOPLIMIT 8
#define IPV6_NEXTHOP 9
#define IPV6_FLOWINFO 11

#define IPV6_UNICAST_HOPS 16
#define IPV6_MULTICAST_IF 17
#define IPV6_MULTICAST_HOPS 18
#define IPV6_MULTICAST_LOOP 19
#define IPV6_ADD_MEMBERSHIP 20
#define IPV6_DROP_MEMBERSHIP 21
#define IPV6_JOIN_GROUP IPV6_ADD_MEMBERSHIP
#define IPV6_LEAVE_GROUP IPV6_DROP_MEMBERSHIP
#define IPV6_ROUTER_ALERT 22
#define IPV6_MTU_DISCOVER 23
#define IPV6_MTU 24
#define IPV6_RECVERR 25
#define IPV6_V6ONLY 26
#define IPV6_JOIN_ANYCAST 27
#define IPV6_LEAVE_ANYCAST 28
#define IPV6_MULTICAST_ALL 29
#define IPV6_ROUTER_ALERT_ISOLATE 30
#define IPV6_RECVERR_RFC4884 31

#define IPV6_RECVPKTINFO 49
#define IPV6_PKTINFO 50
#define IPV6_RECVHOPLIMIT 51
#define IPV6_HOPLIMIT 52
#define IPV6_RECVHOPOPTS 53
#define IPV6_HOPOPTS 54
#define IPV6_RTHDRDSTOPTS 55
#define IPV6_RECVRTHDR 56
#define IPV6_RTHDR 57
#define IPV6_RECVDSTOPTS 58
#define IPV6_DSTOPTS 59
#define IPV6_RECVPATHMTU 60
#define IPV6_PATHMTU 61
#define IPV6_DONTFRAG 62

#define IPV6_RECVTCLASS 66
#define IPV6_TCLASS 67

#define IPV6_AUTOFLOWLABEL 70
#define IPV6_ADDR_PREFERENCES 72
#define IPV6_MINHOPCOUNT 73
#define IPV6_ORIGDSTADDR 74
#define IPV6_RECVORIGDSTADDR IPV6_ORIGDSTADDR
#define IPV6_TRANSPARENT 75
#define IPV6_UNICAST_IF 76
#define IPV6_RECVFRAGSIZE 77
#define IPV6_FREEBIND 78

// What IPV6_MTU_DISCOVER may be set to.
#define IPV6_PMTUDISC_DONT 0
#define IPV6_PMTUDISC_WANT 1
#define IPV6_PMTUDISC_DO 2
#define IPV6_PMTUDISC_PROBE 3
#define IPV6_PMTUDISC_INTERFACE 4
#define IPV6_PMTUDISC_OMIT 5

#endif // LLVM_LIBC_MACROS_NETINET_IN_MACROS_H
