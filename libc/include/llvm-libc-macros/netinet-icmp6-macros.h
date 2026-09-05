//===-- Macros defined in netinet/icmp6.h header file ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_NETINET_ICMP6_MACROS_H
#define LLVM_LIBC_MACROS_NETINET_ICMP6_MACROS_H

#include "endian-macros.h"

// The socket option which sets which message types a raw socket sees.
#define ICMP6_FILTER 1

// What a filter entry says about a type.
#define ICMP6_FILTER_BLOCK 1
#define ICMP6_FILTER_PASS 2
#define ICMP6_FILTER_BLOCKOTHERS 3
#define ICMP6_FILTER_PASSONLY 4

// The messages which report a problem.
#define ICMP6_DST_UNREACH 1
#define ICMP6_PACKET_TOO_BIG 2
#define ICMP6_TIME_EXCEEDED 3
#define ICMP6_PARAM_PROB 4

// A type with this bit set carries information rather than an error.
#define ICMP6_INFOMSG_MASK 0x80

#define ICMP6_ECHO_REQUEST 128
#define ICMP6_ECHO_REPLY 129

// Multicast listener discovery.
#define MLD_LISTENER_QUERY 130
#define MLD_LISTENER_REPORT 131
#define MLD_LISTENER_REDUCTION 132

// Why the destination could not be reached.
#define ICMP6_DST_UNREACH_NOROUTE 0
#define ICMP6_DST_UNREACH_ADMIN 1
#define ICMP6_DST_UNREACH_BEYONDSCOPE 2
#define ICMP6_DST_UNREACH_ADDR 3
#define ICMP6_DST_UNREACH_NOPORT 4

#define ICMP6_TIME_EXCEED_TRANSIT 0
#define ICMP6_TIME_EXCEED_REASSEMBLY 1

#define ICMP6_PARAMPROB_HEADER 0
#define ICMP6_PARAMPROB_NEXTHEADER 1
#define ICMP6_PARAMPROB_OPTION 2

// Neighbour discovery.
#define ND_ROUTER_SOLICIT 133
#define ND_ROUTER_ADVERT 134
#define ND_NEIGHBOR_SOLICIT 135
#define ND_NEIGHBOR_ADVERT 136
#define ND_REDIRECT 137

// What a router advertisement says about how to configure.
#define ND_RA_FLAG_MANAGED 0x80
#define ND_RA_FLAG_OTHER 0x40
#define ND_RA_FLAG_HOME_AGENT 0x20

// A neighbour advertisement carries its flags in the top of a word which
// stays in network order, so which end they sit at depends on the machine.
#if __BYTE_ORDER == __BIG_ENDIAN
#define ND_NA_FLAG_ROUTER 0x80000000
#define ND_NA_FLAG_SOLICITED 0x40000000
#define ND_NA_FLAG_OVERRIDE 0x20000000
#else
#define ND_NA_FLAG_ROUTER 0x00000080
#define ND_NA_FLAG_SOLICITED 0x00000040
#define ND_NA_FLAG_OVERRIDE 0x00000020
#endif

// Which option follows a neighbour discovery message.
#define ND_OPT_SOURCE_LINKADDR 1
#define ND_OPT_TARGET_LINKADDR 2
#define ND_OPT_PREFIX_INFORMATION 3
#define ND_OPT_REDIRECTED_HEADER 4
#define ND_OPT_MTU 5
#define ND_OPT_RTR_ADV_INTERVAL 7
#define ND_OPT_HOME_AGENT_INFO 8

// What a prefix information option says the prefix may be used for.
#define ND_OPT_PI_FLAG_ONLINK 0x80
#define ND_OPT_PI_FLAG_AUTO 0x40
#define ND_OPT_PI_FLAG_RADDR 0x20

#define ICMP6_ROUTER_RENUMBERING 138

// Setting and reading a filter. The bit for a type sits in the word its
// number selects.
#define ICMP6_FILTER_WILLPASS(type, filterp)                                   \
  ((((filterp)->icmp6_filt[(type) >> 5]) & (1U << ((type) & 31))) != 0)
#define ICMP6_FILTER_WILLBLOCK(type, filterp)                                  \
  ((((filterp)->icmp6_filt[(type) >> 5]) & (1U << ((type) & 31))) == 0)
#define ICMP6_FILTER_SETPASS(type, filterp)                                    \
  ((((filterp)->icmp6_filt[(type) >> 5]) |= (1U << ((type) & 31))))
#define ICMP6_FILTER_SETBLOCK(type, filterp)                                   \
  ((((filterp)->icmp6_filt[(type) >> 5]) &= ~(1U << ((type) & 31))))
#define ICMP6_FILTER_SETPASSALL(filterp)                                       \
  do {                                                                         \
    int __i;                                                                   \
    for (__i = 0; __i < 8; ++__i)                                              \
      (filterp)->icmp6_filt[__i] = 0xFFFFFFFFU;                                \
  } while (0)
#define ICMP6_FILTER_SETBLOCKALL(filterp)                                      \
  do {                                                                         \
    int __i;                                                                   \
    for (__i = 0; __i < 8; ++__i)                                              \
      (filterp)->icmp6_filt[__i] = 0;                                          \
  } while (0)

#endif // LLVM_LIBC_MACROS_NETINET_ICMP6_MACROS_H
