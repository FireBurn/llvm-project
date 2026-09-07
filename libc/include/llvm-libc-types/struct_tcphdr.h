//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct tcphdr.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_TCPHDR_H
#define LLVM_LIBC_TYPES_STRUCT_TCPHDR_H

#include "../llvm-libc-macros/endian-macros.h"
#include "../llvm-libc-macros/stdint-macros.h"
#include "tcp_seq.h"

// The TCP header as it sits on the wire, laid out by RFC 793. The same
// fields go by two sets of names: the ones BSD gave them and the ones Linux
// gave them. Code exists that uses each, so both are here, over the same
// storage.
struct tcphdr {
  union {
    struct {
      uint16_t th_sport;
      uint16_t th_dport;
      tcp_seq th_seq;
      tcp_seq th_ack;
#if __BYTE_ORDER == __LITTLE_ENDIAN
      uint8_t th_x2 : 4;
      uint8_t th_off : 4;
#else
      uint8_t th_off : 4;
      uint8_t th_x2 : 4;
#endif
      uint8_t th_flags;
      uint16_t th_win;
      uint16_t th_sum;
      uint16_t th_urp;
    };
    struct {
      uint16_t source;
      uint16_t dest;
      uint32_t seq;
      uint32_t ack_seq;
#if __BYTE_ORDER == __LITTLE_ENDIAN
      uint16_t res1 : 4;
      uint16_t doff : 4;
      uint16_t fin : 1;
      uint16_t syn : 1;
      uint16_t rst : 1;
      uint16_t psh : 1;
      uint16_t ack : 1;
      uint16_t urg : 1;
      uint16_t res2 : 2;
#else
      uint16_t doff : 4;
      uint16_t res1 : 4;
      uint16_t res2 : 2;
      uint16_t urg : 1;
      uint16_t ack : 1;
      uint16_t psh : 1;
      uint16_t rst : 1;
      uint16_t syn : 1;
      uint16_t fin : 1;
#endif
      uint16_t window;
      uint16_t check;
      uint16_t urg_ptr;
    };
  };
};

#endif // LLVM_LIBC_TYPES_STRUCT_TCPHDR_H
