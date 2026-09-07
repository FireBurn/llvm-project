//===-- Macros defined in net/if_arp.h header file ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_NET_IF_ARP_MACROS_H
#define LLVM_LIBC_MACROS_NET_IF_ARP_MACROS_H

// What is known about one entry of the address table.
#define ATF_COM 0x02         // The hardware address is known.
#define ATF_PERM 0x04        // The entry does not time out.
#define ATF_PUBL 0x08        // This host answers for the address.
#define ATF_USETRAILERS 0x10 // Trailer encapsulation was asked for.
#define ATF_NETMASK 0x20     // The entry stands for a range of addresses.
#define ATF_DONTPUB 0x40     // This host does not answer for the address.
#define ATF_MAGIC 0x80       // The entry was added by the kernel itself.

// What an address resolution daemon is being asked to do.
#define ARPD_UPDATE 0x01
#define ARPD_LOOKUP 0x02
#define ARPD_FLUSH 0x03

// The largest hardware address any of the above can carry.
#define MAX_ADDR_LEN 7

#endif // LLVM_LIBC_MACROS_NET_IF_ARP_MACROS_H
