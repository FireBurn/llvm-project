//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct icmp6_hdr and the neighbour discovery messages
/// built on it.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_ICMP6_HDR_H
#define LLVM_LIBC_TYPES_STRUCT_ICMP6_HDR_H

#include "../llvm-libc-macros/stdint-macros.h"
#include "struct_in6_addr.h"

// The header every ICMPv6 message begins with. What the four octets after
// the checksum mean depends on the type, so they are reached through the
// names each message gives them.
struct icmp6_hdr {
  uint8_t icmp6_type;
  uint8_t icmp6_code;
  uint16_t icmp6_cksum;
  union {
    uint32_t icmp6_un_data32[1];
    uint16_t icmp6_un_data16[2];
    uint8_t icmp6_un_data8[4];
  } icmp6_dataun;
};

#define icmp6_data32 icmp6_dataun.icmp6_un_data32
#define icmp6_data16 icmp6_dataun.icmp6_un_data16
#define icmp6_data8 icmp6_dataun.icmp6_un_data8
#define icmp6_pptr icmp6_data32[0]
#define icmp6_mtu icmp6_data32[0]
#define icmp6_id icmp6_data16[0]
#define icmp6_seq icmp6_data16[1]
#define icmp6_maxdelay icmp6_data16[0]

struct nd_router_solicit {
  struct icmp6_hdr nd_rs_hdr;
  // Options may follow.
};

#define nd_rs_type nd_rs_hdr.icmp6_type
#define nd_rs_code nd_rs_hdr.icmp6_code
#define nd_rs_cksum nd_rs_hdr.icmp6_cksum
#define nd_rs_reserved nd_rs_hdr.icmp6_data32[0]

struct nd_router_advert {
  struct icmp6_hdr nd_ra_hdr;
  uint32_t nd_ra_reachable;
  uint32_t nd_ra_retransmit;
  // Options may follow.
};

#define nd_ra_type nd_ra_hdr.icmp6_type
#define nd_ra_code nd_ra_hdr.icmp6_code
#define nd_ra_cksum nd_ra_hdr.icmp6_cksum
#define nd_ra_curhoplimit nd_ra_hdr.icmp6_data8[0]
#define nd_ra_flags_reserved nd_ra_hdr.icmp6_data8[1]
#define nd_ra_router_lifetime nd_ra_hdr.icmp6_data16[1]

struct nd_neighbor_solicit {
  struct icmp6_hdr nd_ns_hdr;
  struct in6_addr nd_ns_target;
  // Options may follow.
};

#define nd_ns_type nd_ns_hdr.icmp6_type
#define nd_ns_code nd_ns_hdr.icmp6_code
#define nd_ns_cksum nd_ns_hdr.icmp6_cksum
#define nd_ns_reserved nd_ns_hdr.icmp6_data32[0]

struct nd_neighbor_advert {
  struct icmp6_hdr nd_na_hdr;
  struct in6_addr nd_na_target;
  // Options may follow.
};

#define nd_na_type nd_na_hdr.icmp6_type
#define nd_na_code nd_na_hdr.icmp6_code
#define nd_na_cksum nd_na_hdr.icmp6_cksum
#define nd_na_flags_reserved nd_na_hdr.icmp6_data32[0]

struct nd_redirect {
  struct icmp6_hdr nd_rd_hdr;
  struct in6_addr nd_rd_target;
  struct in6_addr nd_rd_dst;
  // Options may follow.
};

#define nd_rd_type nd_rd_hdr.icmp6_type
#define nd_rd_code nd_rd_hdr.icmp6_code
#define nd_rd_cksum nd_rd_hdr.icmp6_cksum
#define nd_rd_reserved nd_rd_hdr.icmp6_data32[0]

// The header every neighbour discovery option begins with. The length is
// in units of eight octets and covers the header too.
struct nd_opt_hdr {
  uint8_t nd_opt_type;
  uint8_t nd_opt_len;
};

struct nd_opt_prefix_info {
  uint8_t nd_opt_pi_type;
  uint8_t nd_opt_pi_len;
  uint8_t nd_opt_pi_prefix_len;
  uint8_t nd_opt_pi_flags_reserved;
  uint32_t nd_opt_pi_valid_time;
  uint32_t nd_opt_pi_preferred_time;
  uint32_t nd_opt_pi_reserved2;
  struct in6_addr nd_opt_pi_prefix;
};

struct nd_opt_rd_hdr {
  uint8_t nd_opt_rh_type;
  uint8_t nd_opt_rh_len;
  uint16_t nd_opt_rh_reserved1;
  uint32_t nd_opt_rh_reserved2;
  // The redirected packet follows.
};

struct nd_opt_mtu {
  uint8_t nd_opt_mtu_type;
  uint8_t nd_opt_mtu_len;
  uint16_t nd_opt_mtu_reserved;
  uint32_t nd_opt_mtu_mtu;
};

struct nd_opt_adv_interval {
  uint8_t nd_opt_adv_interval_type;
  uint8_t nd_opt_adv_interval_len;
  uint16_t nd_opt_adv_interval_reserved;
  uint32_t nd_opt_adv_interval_ival;
};

struct nd_opt_home_agent_info {
  uint8_t nd_opt_home_agent_info_type;
  uint8_t nd_opt_home_agent_info_len;
  uint16_t nd_opt_home_agent_info_reserved;
  uint16_t nd_opt_home_agent_info_preference;
  uint16_t nd_opt_home_agent_info_lifetime;
};

// The filter a raw ICMPv6 socket is set with, one bit per message type.
struct icmp6_filter {
  uint32_t icmp6_filt[8];
};

#endif // LLVM_LIBC_TYPES_STRUCT_ICMP6_HDR_H
