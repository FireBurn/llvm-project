//===-- Macros defined in netinet/ip_icmp.h header file -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_NETINET_IP_ICMP_MACROS_H
#define LLVM_LIBC_MACROS_NETINET_IP_ICMP_MACROS_H

// The kinds of ICMP message, and what each kind says about itself.
#define ICMP_ECHOREPLY 0       // Echo Reply
#define ICMP_DEST_UNREACH 3    // Destination Unreachable
#define ICMP_SOURCE_QUENCH 4   // Source Quench
#define ICMP_REDIRECT 5        // Redirect (change route)
#define ICMP_ECHO 8            // Echo Request
#define ICMP_TIME_EXCEEDED 11  // Time Exceeded
#define ICMP_PARAMETERPROB 12  // Parameter Problem
#define ICMP_TIMESTAMP 13      // Timestamp Request
#define ICMP_TIMESTAMPREPLY 14 // Timestamp Reply
#define ICMP_INFO_REQUEST 15   // Information Request
#define ICMP_INFO_REPLY 16     // Information Reply
#define ICMP_ADDRESS 17        // Address Mask Request
#define ICMP_ADDRESSREPLY 18   // Address Mask Reply
#define ICMP_NET_UNREACH 0     // Network Unreachable
#define ICMP_HOST_UNREACH 1    // Host Unreachable
#define ICMP_PROT_UNREACH 2    // Protocol Unreachable
#define ICMP_PORT_UNREACH 3    // Port Unreachable
#define ICMP_FRAG_NEEDED 4     // Fragmentation Needed/DF set
#define ICMP_SR_FAILED 5       // Source Route failed
#define ICMP_NET_UNKNOWN 6
#define ICMP_HOST_UNKNOWN 7
#define ICMP_HOST_ISOLATED 8
#define ICMP_NET_ANO 9
#define ICMP_HOST_ANO 10
#define ICMP_NET_UNR_TOS 11
#define ICMP_HOST_UNR_TOS 12
#define ICMP_PKT_FILTERED 13   // Packet filtered
#define ICMP_PREC_VIOLATION 14 // Precedence violation
#define ICMP_PREC_CUTOFF 15    // Precedence cut off
#define ICMP_REDIR_NET 0       // Redirect Net
#define ICMP_REDIR_HOST 1      // Redirect Host
#define ICMP_REDIR_NETTOS 2    // Redirect Net for TOS
#define ICMP_REDIR_HOSTTOS 3   // Redirect Host for TOS
#define ICMP_EXC_TTL 0         // TTL count exceeded
#define ICMP_EXC_FRAGTIME 1    // Fragment Reass time exceeded
#define ICMP_EXT_ECHO 42
#define ICMP_EXT_ECHOREPLY 43
#define ICMP_EXT_CODE_MAL_QUERY 1    // Malformed Query
#define ICMP_EXT_CODE_NO_IF 2        // No such Interface
#define ICMP_EXT_CODE_NO_TABLE_ENT 3 // No table entry
#define ICMP_EXT_CODE_MULT_IFS 4     // Multiple Interfaces Satisfy Query
#define ICMP_EXT_ECHOREPLY_ACTIVE (1
#define ICMP_EXT_ECHOREPLY_IPV4  (1
#define ICMP_EXT_ECHOREPLY_IPV6 1 // ipv6 bit in reply
#define ICMP_EXT_ECHO_CTYPE_NAME 1
#define ICMP_EXT_ECHO_CTYPE_INDEX 2
#define ICMP_EXT_ECHO_CTYPE_ADDR 3
#define ICMP_AFI_IP 1  // Address Family Identifier for IPV4
#define ICMP_AFI_IP6 2 // Address Family Identifier for IPV6
#define ICMP_MINLEN 8  // abs minimum
#define ICMP_TSLEN               (8
#define ICMP_MASKLEN 12 // address mask
#define ICMP_ADVLENMIN           (8
#define ICMP_UNREACH 3                    // dest unreachable, codes:
#define ICMP_SOURCEQUENCH 4               // packet lost, slow down
#define ICMP_ROUTERADVERT 9               // router advertisement
#define ICMP_ROUTERSOLICIT 10             // router solicitation
#define ICMP_TIMXCEED 11                  // time exceeded, code:
#define ICMP_PARAMPROB 12                 // ip header bad
#define ICMP_TSTAMP 13                    // timestamp request
#define ICMP_TSTAMPREPLY 14               // timestamp reply
#define ICMP_IREQ 15                      // information request
#define ICMP_IREQREPLY 16                 // information reply
#define ICMP_MASKREQ 17                   // address mask request
#define ICMP_MASKREPLY 18                 // address mask reply
#define ICMP_MAXTYPE 18                   // UNREACH codes
#define ICMP_UNREACH_NET 0                // bad net
#define ICMP_UNREACH_HOST 1               // bad host
#define ICMP_UNREACH_PROTOCOL 2           // bad protocol
#define ICMP_UNREACH_PORT 3               // bad port
#define ICMP_UNREACH_NEEDFRAG 4           // IP_DF caused drop
#define ICMP_UNREACH_SRCFAIL 5            // src route failed
#define ICMP_UNREACH_NET_UNKNOWN 6        // unknown net
#define ICMP_UNREACH_HOST_UNKNOWN 7       // unknown host
#define ICMP_UNREACH_ISOLATED 8           // src host isolated
#define ICMP_UNREACH_NET_PROHIB 9         // net denied
#define ICMP_UNREACH_HOST_PROHIB 10       // host denied
#define ICMP_UNREACH_TOSNET 11            // bad tos for net
#define ICMP_UNREACH_TOSHOST 12           // bad tos for host
#define ICMP_UNREACH_FILTER_PROHIB 13     // admin prohib
#define ICMP_UNREACH_HOST_PRECEDENCE 14   // host prec vio.
#define ICMP_UNREACH_PRECEDENCE_CUTOFF 15 // prec cutoff
#define ICMP_REDIRECT_NET 0               // for network
#define ICMP_REDIRECT_HOST 1              // for host
#define ICMP_REDIRECT_TOSNET 2            // for tos and net
#define ICMP_REDIRECT_TOSHOST 3           // for tos and host
#define ICMP_TIMXCEED_INTRANS 0           // ttl==0 in transit
#define ICMP_TIMXCEED_REASS 1             // ttl==0 in reass
#define ICMP_PARAMPROB_OPTABSENT 1        // req. opt. absent

// How many kinds there are, and how long the shortest message of each
// shape is.
#define NR_ICMP_TYPES 18
#define ICMP_MINLEN 8
#define ICMP_TSLEN (8 + 3 * sizeof(n_time))
#define ICMP_MASKLEN 12
#define ICMP_ADVLENMIN (8 + sizeof(struct ip) + 8)
#endif // LLVM_LIBC_MACROS_NETINET_IP_ICMP_MACROS_H
