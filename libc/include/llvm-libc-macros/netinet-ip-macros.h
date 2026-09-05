//===-- Macros defined in netinet/ip.h header file ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_NETINET_IP_MACROS_H
#define LLVM_LIBC_MACROS_NETINET_IP_MACROS_H

// The type of service field, in its original reading: three bits of
// precedence and four of what to optimise for.
#define IPTOS_TOS_MASK 0x1E
#define IPTOS_TOS(tos) ((tos) & IPTOS_TOS_MASK)
#define IPTOS_LOWDELAY 0x10
#define IPTOS_THROUGHPUT 0x08
#define IPTOS_RELIABILITY 0x04
#define IPTOS_LOWCOST 0x02
#define IPTOS_MINCOST IPTOS_LOWCOST

#define IPTOS_PREC_MASK 0xE0
#define IPTOS_PREC(tos) ((tos) & IPTOS_PREC_MASK)
#define IPTOS_PREC_NETCONTROL 0xE0
#define IPTOS_PREC_INTERNETCONTROL 0xC0
#define IPTOS_PREC_CRITIC_ECP 0xA0
#define IPTOS_PREC_FLASHOVERRIDE 0x80
#define IPTOS_PREC_FLASH 0x60
#define IPTOS_PREC_IMMEDIATE 0x40
#define IPTOS_PREC_PRIORITY 0x20
#define IPTOS_PREC_ROUTINE 0x00

// The same octet in its later reading: six bits of differentiated services
// code point and two of explicit congestion notification.
#define IPTOS_DSCP_MASK 0xfc
#define IPTOS_DSCP(tos) ((tos) & IPTOS_DSCP_MASK)
#define IPTOS_DSCP_AF11 0x28
#define IPTOS_DSCP_AF12 0x30
#define IPTOS_DSCP_AF13 0x38
#define IPTOS_DSCP_AF21 0x48
#define IPTOS_DSCP_AF22 0x50
#define IPTOS_DSCP_AF23 0x58
#define IPTOS_DSCP_AF31 0x68
#define IPTOS_DSCP_AF32 0x70
#define IPTOS_DSCP_AF33 0x78
#define IPTOS_DSCP_AF41 0x88
#define IPTOS_DSCP_AF42 0x90
#define IPTOS_DSCP_AF43 0x98
#define IPTOS_DSCP_EF 0xb8

#define IPTOS_CLASS_MASK 0xe0
#define IPTOS_CLASS(class) ((class) & IPTOS_CLASS_MASK)
#define IPTOS_CLASS_CS0 0x00
#define IPTOS_CLASS_CS1 0x20
#define IPTOS_CLASS_CS2 0x40
#define IPTOS_CLASS_CS3 0x60
#define IPTOS_CLASS_CS4 0x80
#define IPTOS_CLASS_CS5 0xa0
#define IPTOS_CLASS_CS6 0xc0
#define IPTOS_CLASS_CS7 0xe0
#define IPTOS_CLASS_DEFAULT IPTOS_CLASS_CS0

#define IPTOS_ECN_MASK 0x03
#define IPTOS_ECN(tos) ((tos) & IPTOS_ECN_MASK)
#define IPTOS_ECN_NOT_ECT 0x00
#define IPTOS_ECN_ECT1 0x01
#define IPTOS_ECN_ECT0 0x02
#define IPTOS_ECN_CE 0x03

// The options which may follow the header.
#define IPOPT_COPY 0x80
#define IPOPT_CLASS_MASK 0x60
#define IPOPT_NUMBER_MASK 0x1f
#define IPOPT_COPIED(o) ((o) & IPOPT_COPY)
#define IPOPT_CLASS(o) ((o) & IPOPT_CLASS_MASK)
#define IPOPT_NUMBER(o) ((o) & IPOPT_NUMBER_MASK)

#define IPOPT_CONTROL 0x00
#define IPOPT_RESERVED1 0x20
#define IPOPT_MEASUREMENT 0x40
#define IPOPT_RESERVED2 0x60

#define IPOPT_END 0
#define IPOPT_EOL IPOPT_END
#define IPOPT_NOOP 1
#define IPOPT_NOP IPOPT_NOOP
#define IPOPT_SEC 130
#define IPOPT_LSRR 131
#define IPOPT_TIMESTAMP 68
#define IPOPT_TS IPOPT_TIMESTAMP
#define IPOPT_RR 7
#define IPOPT_SID 136
#define IPOPT_SSRR 137
#define IPOPT_RA 148

#define IPOPT_OPTVAL 0
#define IPOPT_OLEN 1
#define IPOPT_OFFSET 2
#define IPOPT_MINOFF 4

#define MAX_IPOPTLEN 40

// The version this header carries, and the limits on its length.
#define IPVERSION 4
#define IP_MAXPACKET 65535

// What the timestamp option is recording.
#define IPOPT_TS_TSONLY 0
#define IPOPT_TS_TSANDADDR 1
#define IPOPT_TS_PRESPEC 3

// The security option's classifications.
#define IPOPT_SECUR_UNCLASS 0x0000
#define IPOPT_SECUR_CONFID 0xf135
#define IPOPT_SECUR_EFTO 0x789a
#define IPOPT_SECUR_MMMM 0xbc4d
#define IPOPT_SECUR_RESTR 0xaf13
#define IPOPT_SECUR_SECRET 0xd788
#define IPOPT_SECUR_TOPSECRET 0x6bc5

// The default and greatest time to live.
#define MAXTTL 255
#define IPDEFTTL 64
#define IPFRAGTTL 60
#define IPTTLDEC 1

#define IP_MSS 576

#endif // LLVM_LIBC_MACROS_NETINET_IP_MACROS_H
