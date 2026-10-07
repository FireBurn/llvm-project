//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct icmp.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_ICMP_H
#define LLVM_LIBC_TYPES_STRUCT_ICMP_H

#include "../llvm-libc-macros/stdint-macros.h"
#include "struct_in_addr.h"
#include "struct_ip.h"

// A router advertisement entry.
struct icmp_ra_addr {
  uint32_t ira_addr;
  uint32_t ira_preference;
};

// An ICMP message under its BSD names: the fixed header, a word whose meaning
// depends on the type, and then the data.
struct icmp {
  uint8_t icmp_type;   // Type of message.
  uint8_t icmp_code;   // Subcode.
  uint16_t icmp_cksum; // Checksum.
  union {
    unsigned char ih_pptr;  // Parameter problem.
    struct in_addr ih_gwaddr; // Redirect gateway.
    struct ih_idseq {         // Echo.
      uint16_t icd_id;
      uint16_t icd_seq;
    } ih_idseq;
    uint32_t ih_void;
    struct ih_pmtu { // Path MTU discovery, RFC 1191.
      uint16_t ipm_void;
      uint16_t ipm_nextmtu;
    } ih_pmtu;
    struct ih_rtradv { // Router advertisement.
      uint8_t irt_num_addrs;
      uint8_t irt_wpa;
      uint16_t irt_lifetime;
    } ih_rtradv;
  } icmp_hun;
#define icmp_pptr icmp_hun.ih_pptr
#define icmp_gwaddr icmp_hun.ih_gwaddr
#define icmp_id icmp_hun.ih_idseq.icd_id
#define icmp_seq icmp_hun.ih_idseq.icd_seq
#define icmp_void icmp_hun.ih_void
#define icmp_pmvoid icmp_hun.ih_pmtu.ipm_void
#define icmp_nextmtu icmp_hun.ih_pmtu.ipm_nextmtu
#define icmp_num_addrs icmp_hun.ih_rtradv.irt_num_addrs
#define icmp_wpa icmp_hun.ih_rtradv.irt_wpa
#define icmp_lifetime icmp_hun.ih_rtradv.irt_lifetime
  union {
    struct {
      uint32_t its_otime;
      uint32_t its_rtime;
      uint32_t its_ttime;
    } id_ts;
    struct {
      struct ip idi_ip; // Options and 64 bits of data follow.
    } id_ip;
    struct icmp_ra_addr id_radv;
    uint32_t id_mask;
    uint8_t id_data[1];
  } icmp_dun;
#define icmp_otime icmp_dun.id_ts.its_otime
#define icmp_rtime icmp_dun.id_ts.its_rtime
#define icmp_ttime icmp_dun.id_ts.its_ttime
#define icmp_ip icmp_dun.id_ip.idi_ip
#define icmp_radv icmp_dun.id_radv
#define icmp_mask icmp_dun.id_mask
#define icmp_data icmp_dun.id_data
};

#endif // LLVM_LIBC_TYPES_STRUCT_ICMP_H
