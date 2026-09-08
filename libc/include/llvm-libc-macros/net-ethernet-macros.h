//===-- Macros defined in net/ethernet.h header file ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_NET_ETHERNET_MACROS_H
#define LLVM_LIBC_MACROS_NET_ETHERNET_MACROS_H


// The shape of an Ethernet frame.
#define ETH_ALEN 6         // Octets in one address.
#define ETH_TLEN 2         // Octets in the type field.
#define ETH_HLEN 14        // Octets in the header.
#define ETH_ZLEN 60        // Least octets in a frame, the checksum aside.
#define ETH_DATA_LEN 1500  // Most octets a frame may carry.
#define ETH_FRAME_LEN 1514 // Most octets in a frame, the checksum aside.
#define ETH_FCS_LEN 4      // Octets in the checksum.
#define ETH_MIN_MTU 68
#define ETH_MAX_MTU 0xFFFFU

// The BSD spellings of the same, which is what the structures here use.
#define ETHER_ADDR_LEN ETH_ALEN
#define ETHER_TYPE_LEN ETH_TLEN
#define ETHER_CRC_LEN ETH_FCS_LEN
#define ETHER_HDR_LEN ETH_HLEN
#define ETHER_MIN_LEN (ETH_ZLEN + ETHER_CRC_LEN)
#define ETHER_MAX_LEN (ETH_FRAME_LEN + ETHER_CRC_LEN)
#define ETHER_IS_VALID_LEN(len)                                                \
  ((len) >= ETHER_MIN_LEN && (len) <= ETHER_MAX_LEN)

// What the type field says the frame carries, in the older spelling.
#define ETHERTYPE_PUP 0x0200
#define ETHERTYPE_SPRITE 0x0500
#define ETHERTYPE_IP 0x0800
#define ETHERTYPE_ARP 0x0806
#define ETHERTYPE_REVARP 0x8035
#define ETHERTYPE_AT 0x809B
#define ETHERTYPE_AARP 0x80F3
#define ETHERTYPE_VLAN 0x8100
#define ETHERTYPE_IPX 0x8137
#define ETHERTYPE_IPV6 0x86dd
#define ETHERTYPE_LOOPBACK 0x9000

// A type field below this is a length instead, and the frame carries a
// trailer.
#define ETHERTYPE_TRAIL 0x1000
#define ETHERTYPE_NTRAILER 16

// And the same set in the spelling Linux uses, which is the longer one.
#define ETH_P_LOOP 0x0060
#define ETH_P_PUP 0x0200
#define ETH_P_PUPAT 0x0201
#define ETH_P_TSN 0x22F0
#define ETH_P_ERSPAN2 0x22EB
#define ETH_P_IP 0x0800
#define ETH_P_X25 0x0805
#define ETH_P_ARP 0x0806
#define ETH_P_BPQ 0x08FF
#define ETH_P_IEEEPUP 0x0a00
#define ETH_P_IEEEPUPAT 0x0a01
#define ETH_P_BATMAN 0x4305
#define ETH_P_DEC 0x6000
#define ETH_P_DNA_DL 0x6001
#define ETH_P_DNA_RC 0x6002
#define ETH_P_DNA_RT 0x6003
#define ETH_P_LAT 0x6004
#define ETH_P_DIAG 0x6005
#define ETH_P_CUST 0x6006
#define ETH_P_SCA 0x6007
#define ETH_P_TEB 0x6558
#define ETH_P_RARP 0x8035
#define ETH_P_ATALK 0x809B
#define ETH_P_AARP 0x80F3
#define ETH_P_8021Q 0x8100
#define ETH_P_ERSPAN 0x88BE
#define ETH_P_IPX 0x8137
#define ETH_P_IPV6 0x86DD
#define ETH_P_PAUSE 0x8808
#define ETH_P_SLOW 0x8809
#define ETH_P_WCCP 0x883E
#define ETH_P_MPLS_UC 0x8847
#define ETH_P_MPLS_MC 0x8848
#define ETH_P_ATMMPOA 0x884c
#define ETH_P_PPP_DISC 0x8863
#define ETH_P_PPP_SES 0x8864
#define ETH_P_LINK_CTL 0x886c
#define ETH_P_ATMFATE 0x8884
#define ETH_P_PAE 0x888E
#define ETH_P_PROFINET 0x8892
#define ETH_P_REALTEK 0x8899
#define ETH_P_AOE 0x88A2
#define ETH_P_ETHERCAT 0x88A4
#define ETH_P_8021AD 0x88A8
#define ETH_P_802_EX1 0x88B5
#define ETH_P_MXLGSW 0x88C3
#define ETH_P_PREAUTH 0x88C7
#define ETH_P_TIPC 0x88CA
#define ETH_P_LLDP 0x88CC
#define ETH_P_MRP 0x88E3
#define ETH_P_MACSEC 0x88E5
#define ETH_P_8021AH 0x88E7
#define ETH_P_MVRP 0x88F5
#define ETH_P_1588 0x88F7
#define ETH_P_NCSI 0x88F8
#define ETH_P_PRP 0x88FB
#define ETH_P_CFM 0x8902
#define ETH_P_FCOE 0x8906
#define ETH_P_IBOE 0x8915
#define ETH_P_TDLS 0x890D
#define ETH_P_FIP 0x8914
#define ETH_P_80221 0x8917
#define ETH_P_HSR 0x892F
#define ETH_P_NSH 0x894F
#define ETH_P_LOOPBACK 0x9000
#define ETH_P_QINQ1 0x9100
#define ETH_P_QINQ2 0x9200
#define ETH_P_QINQ3 0x9300
#define ETH_P_YT921X 0x9988
#define ETH_P_EDSA 0xDADA
#define ETH_P_DSA_8021Q 0xDADB
#define ETH_P_DSA_A5PSW 0xE001
#define ETH_P_IFE 0xED3E
#define ETH_P_AF_IUCV 0xFBFB
#define ETH_P_802_3_MIN 0x0600
#define ETH_P_802_3 0x0001
#define ETH_P_AX25 0x0002
#define ETH_P_ALL 0x0003
#define ETH_P_802_2 0x0004
#define ETH_P_SNAP 0x0005
#define ETH_P_DDCMP 0x0006
#define ETH_P_WAN_PPP 0x0007
#define ETH_P_PPP_MP 0x0008
#define ETH_P_LOCALTALK 0x0009
#define ETH_P_CAN 0x000C
#define ETH_P_CANFD 0x000D
#define ETH_P_CANXL 0x000E
#define ETH_P_PPPTALK 0x0010
#define ETH_P_TR_802_2 0x0011
#define ETH_P_MOBITEX 0x0015
#define ETH_P_CONTROL 0x0016
#define ETH_P_IRDA 0x0017
#define ETH_P_ECONET 0x0018
#define ETH_P_HDLC 0x0019
#define ETH_P_ARCNET 0x001A
#define ETH_P_DSA 0x001B
#define ETH_P_TRAILER 0x001C
#define ETH_P_PHONET 0x00F5
#define ETH_P_IEEE802154 0x00F6
#define ETH_P_CAIF 0x00F7
#define ETH_P_XDSA 0x00F8
#define ETH_P_MAP 0x00F9
#define ETH_P_MCTP 0x00FA

#endif // LLVM_LIBC_MACROS_NET_ETHERNET_MACROS_H