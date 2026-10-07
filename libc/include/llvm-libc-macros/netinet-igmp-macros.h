//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Macros defined in netinet/igmp.h header file
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_NETINET_IGMP_MACROS_H
#define LLVM_LIBC_MACROS_NETINET_IGMP_MACROS_H

#define IGMP_MINLEN 8
#define IGMP_MEMBERSHIP_QUERY 0x11       // Membership query.
#define IGMP_V1_MEMBERSHIP_REPORT 0x12   // Version 1 membership report.
#define IGMP_V2_MEMBERSHIP_REPORT 0x16   // Version 2 membership report.
#define IGMP_V2_LEAVE_GROUP 0x17         // Leave-group message.
#define IGMP_DVMRP 0x13                  // DVMRP routing message.
#define IGMP_PIM 0x14                    // PIM routing message.
#define IGMP_TRACE 0x15
#define IGMP_MTRACE_RESP 0x1e            // Traceroute response, to the sender.
#define IGMP_MTRACE 0x1f                 // Multicast traceroute messages.
#define IGMP_MRDISC_ADV 0x30             // From RFC 4286.
#define IGMP_MAX_HOST_REPORT_DELAY 10    // Longest wait before answering a query.
#define IGMP_TIMER_SCALE 10              // The code field counts tenths of a second.
#define IGMP_DELAYING_MEMBER 1
#define IGMP_IDLE_MEMBER 2
#define IGMP_LAZY_MEMBER 3
#define IGMP_SLEEPING_MEMBER 4
#define IGMP_AWAKENING_MEMBER 5
#define IGMP_v1_ROUTER 1
#define IGMP_v2_ROUTER 2
#define IGMP_HOST_MEMBERSHIP_QUERY IGMP_MEMBERSHIP_QUERY
#define IGMP_HOST_MEMBERSHIP_REPORT IGMP_V1_MEMBERSHIP_REPORT
#define IGMP_HOST_NEW_MEMBERSHIP_REPORT IGMP_V2_MEMBERSHIP_REPORT
#define IGMP_HOST_LEAVE_MESSAGE IGMP_V2_LEAVE_GROUP

#endif // LLVM_LIBC_MACROS_NETINET_IGMP_MACROS_H
