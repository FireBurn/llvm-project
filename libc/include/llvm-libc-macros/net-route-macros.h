//===-- Macros defined in net/route.h header file -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_NET_ROUTE_MACROS_H
#define LLVM_LIBC_MACROS_NET_ROUTE_MACROS_H

// What is known about one route.
#define RTF_UP 0x0001         // The route can be used.
#define RTF_GATEWAY 0x0002    // It leads to a gateway rather than a network.
#define RTF_HOST 0x0004       // It names one host rather than a network.
#define RTF_REINSTATE 0x0008  // Bring it back when it has timed out.
#define RTF_DYNAMIC 0x0010    // A redirect created it.
#define RTF_MODIFIED 0x0020   // A redirect changed it.
#define RTF_MTU 0x0040        // It carries an MTU of its own.
#define RTF_MSS RTF_MTU       // What that was called before.
#define RTF_WINDOW 0x0080     // It clamps the window.
#define RTF_IRTT 0x0100       // It states a round trip time to start from.
#define RTF_REJECT 0x0200     // Refuse rather than forward.
#define RTF_STATIC 0x0400     // Someone added it by hand.
#define RTF_XRESOLVE 0x0800   // Something outside the kernel resolves it.
#define RTF_NOFORWARD 0x1000  // Do not forward along it.
#define RTF_THROW 0x2000      // Try the next table.
#define RTF_NOPMTUDISC 0x4000 // Do not set the do-not-fragment bit.

// The ones which only apply to IPv6.
#define RTF_DEFAULT 0x00010000   // Learned from a router advertisement.
#define RTF_ALLONLINK 0x00020000 // Everything is reachable on the link.
#define RTF_ADDRCONF 0x00040000  // An address configuration made it.
#define RTF_LINKRT 0x00100000    // It belongs to one interface.
#define RTF_NONEXTHOP 0x00200000 // There is no gateway to send to.
#define RTF_CACHE 0x01000000     // It is remembered rather than configured.
#define RTF_FLOW 0x02000000      // It applies to one flow.
#define RTF_POLICY 0x04000000    // A policy chose it.
#define RTF_NAT 0x08000000       // It translates addresses.

// What the address at the end of the route is.
#define RTF_LOCAL 0x80000000
#define RTF_INTERFACE 0x40000000
#define RTF_MULTICAST 0x20000000
#define RTF_BROADCAST 0x10000000
#define RTF_ADDRCLASSMASK 0xF8000000

// The old names for the classes of route, which the kernel no longer keeps
// apart but which programs still name.
#define RTCF_VALVE 0x00200000
#define RTCF_MASQ 0x00400000
#define RTCF_NAT 0x00800000
#define RTCF_DOREDIRECT 0x01000000
#define RTCF_LOG 0x02000000
#define RTCF_DIRECTSRC 0x04000000

#define RT_CLASS_UNSPEC 0
#define RT_CLASS_DEFAULT 253
#define RT_CLASS_MAIN 254
#define RT_CLASS_LOCAL 255
#define RT_CLASS_MAX 255

#endif // LLVM_LIBC_MACROS_NET_ROUTE_MACROS_H
