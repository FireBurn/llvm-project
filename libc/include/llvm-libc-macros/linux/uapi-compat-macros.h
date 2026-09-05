//===-- Telling the kernel headers what this libc already defines ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The kernel's UAPI headers define a good deal of what a libc has to define
/// too: the address structures, the protocol numbers, the interface request
/// structures. <linux/libc-compat.h> exists so the two do not fight, and it
/// works both ways round. Each contested definition has a guard named
/// __UAPI_DEF_something; the kernel defines it to 1 unless it is already
/// set, and skips its own definition when it is 0.
///
/// So a header here which defines one of them says so by setting the guard
/// to zero, and guards its own definition on the guard as well, in case the
/// kernel header was read first.
///
//===----------------------------------------------------------------------===//

// This header has no include guard on purpose. Which guards it sets
// depends on which headers have been read so far, so it has to be read
// again by each of them. Setting a guard twice is harmless: each is set
// only if it is not set already.

// Which of these apply depends on which headers here have been read, the
// same way <linux/libc-compat.h> keys on glibc's include guards. A guard is
// only set to zero once the header which defines that thing has been
// included, so a program which reaches for a kernel header on its own still
// gets the kernel's definitions.

#ifdef _LLVM_LIBC_NETINET_IN_H
#ifndef __UAPI_DEF_IN_IPPROTO
#define __UAPI_DEF_IN_IPPROTO 0
#endif
#ifndef __UAPI_DEF_IN_ADDR
#define __UAPI_DEF_IN_ADDR 0
#endif
#ifndef __UAPI_DEF_IN_CLASS
#define __UAPI_DEF_IN_CLASS 0
#endif
#ifndef __UAPI_DEF_SOCKADDR_IN
#define __UAPI_DEF_SOCKADDR_IN 0
#endif
#ifndef __UAPI_DEF_IP_MREQ
#define __UAPI_DEF_IP_MREQ 0
#endif
#ifndef __UAPI_DEF_IN_PKTINFO
#define __UAPI_DEF_IN_PKTINFO 0
#endif
#ifndef __UAPI_DEF_IN6_ADDR
#define __UAPI_DEF_IN6_ADDR 0
#endif
#ifndef __UAPI_DEF_IN6_ADDR_ALT
#define __UAPI_DEF_IN6_ADDR_ALT 0
#endif
#ifndef __UAPI_DEF_SOCKADDR_IN6
#define __UAPI_DEF_SOCKADDR_IN6 0
#endif
#ifndef __UAPI_DEF_IPV6_MREQ
#define __UAPI_DEF_IPV6_MREQ 0
#endif
#ifndef __UAPI_DEF_IPPROTO_V6
#define __UAPI_DEF_IPPROTO_V6 0
#endif
#ifndef __UAPI_DEF_IPV6_OPTIONS
#define __UAPI_DEF_IPV6_OPTIONS 0
#endif
#ifndef __UAPI_DEF_IN6_PKTINFO
#define __UAPI_DEF_IN6_PKTINFO 0
#endif
#ifndef __UAPI_DEF_IP6_MTUINFO
#define __UAPI_DEF_IP6_MTUINFO 0
#endif
#endif // _LLVM_LIBC_NETINET_IN_H

#ifdef _LLVM_LIBC_NET_IF_H
#ifndef __UAPI_DEF_IF_IFCONF
#define __UAPI_DEF_IF_IFCONF 0
#endif
#ifndef __UAPI_DEF_IF_IFMAP
#define __UAPI_DEF_IF_IFMAP 0
#endif
#ifndef __UAPI_DEF_IF_IFNAMSIZ
#define __UAPI_DEF_IF_IFNAMSIZ 0
#endif
#ifndef __UAPI_DEF_IF_IFREQ
#define __UAPI_DEF_IF_IFREQ 0
#endif
#ifndef __UAPI_DEF_IF_NET_DEVICE_FLAGS
#define __UAPI_DEF_IF_NET_DEVICE_FLAGS 0
#endif
#ifndef __UAPI_DEF_IF_NET_DEVICE_FLAGS_LOWER_UP_DORMANT_ECHO
#define __UAPI_DEF_IF_NET_DEVICE_FLAGS_LOWER_UP_DORMANT_ECHO 0
#endif
#endif // _LLVM_LIBC_NET_IF_H

// struct ethhdr is the kernel's, not this libc's: <net/ethernet.h> defines
// struct ether_header and leaves the other to <linux/if_ether.h>. So
// __UAPI_DEF_ETHHDR is left alone.
