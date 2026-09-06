//===-- Macros defined in netpacket/packet.h ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_PACKET_MACROS_H
#define LLVM_LIBC_MACROS_PACKET_MACROS_H

// Why a frame arrived: who it was addressed to, or that it is on its way out.
#define PACKET_HOST 0
#define PACKET_BROADCAST 1
#define PACKET_MULTICAST 2
#define PACKET_OTHERHOST 3
#define PACKET_OUTGOING 4
#define PACKET_LOOPBACK 5
#define PACKET_USER 6
#define PACKET_KERNEL 7
#define PACKET_FASTROUTE 6

// What may be set on such a socket.
#define PACKET_ADD_MEMBERSHIP 1
#define PACKET_DROP_MEMBERSHIP 2
#define PACKET_RECV_OUTPUT 3
#define PACKET_RX_RING 5
#define PACKET_STATISTICS 6
#define PACKET_COPY_THRESH 7
#define PACKET_AUXDATA 8
#define PACKET_ORIGDEV 9
#define PACKET_VERSION 10
#define PACKET_HDRLEN 11
#define PACKET_RESERVE 12
#define PACKET_TX_RING 13
#define PACKET_LOSS 14
#define PACKET_VNET_HDR 15
#define PACKET_TX_TIMESTAMP 16
#define PACKET_TIMESTAMP 17
#define PACKET_FANOUT 18
#define PACKET_QDISC_BYPASS 20

// Which addresses a device is asked to listen for.
#define PACKET_MR_MULTICAST 0
#define PACKET_MR_PROMISC 1
#define PACKET_MR_ALLMULTI 2
#define PACKET_MR_UNICAST 3

#endif // LLVM_LIBC_MACROS_PACKET_MACROS_H
