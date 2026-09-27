//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct __res_state.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT___RES_STATE_H
#define LLVM_LIBC_TYPES_STRUCT___RES_STATE_H

#include "../llvm-libc-macros/resolv-macros.h"
#include "../llvm-libc-macros/stdint-macros.h"
#include "struct_in_addr.h"
#include "struct_sockaddr_in.h"
#include "struct_sockaddr_in6.h"

// What the resolver was told and what it is partway through. A program may
// read this to see which servers were found and set the options before asking
// anything; the fields are the ones BIND gave it, since that is what code
// reaching in here expects to find.
struct __res_state {
  int retrans; // Seconds to wait for an answer.
  int retry;   // How many times to ask before giving up.
  unsigned long options;
  int nscount; // How many of nsaddr_list are filled in.
  struct sockaddr_in nsaddr_list[MAXNS];
  unsigned short id;           // The identifier the next query will carry.
  char *dnsrch[MAXDNSRCH + 1]; // The domains an unqualified name is tried in.
  char defdname[256];          // The domain this machine is in.
  unsigned long pfcode;        // Which RES_PRF_ messages to print.
  unsigned ndots : 4; // Dots a name needs before it is tried as it stands.
  unsigned nsort : 4; // How many of sort_list are filled in.
  unsigned ipv6_unavail : 1;
  unsigned unused : 23;
  struct {
    struct in_addr addr;
    uint32_t mask;
  } sort_list[MAXRESOLVSORT];
  // glibc's layout from here on, which it keeps for programs that set aside
  // a state of their own: two hooks it no longer calls, and the servers
  // reached over IPv6 with the sockets open to each.
  void *__glibc_unused_qhook;
  void *__glibc_unused_rhook;
  int res_h_errno; // What the last lookup through this state set.
  int _vcsock;     // The connection res_send keeps where it is asked to.
  unsigned int _flags;
  union {
    char pad[52];
    struct {
      uint16_t nscount;
      uint16_t nsmap[MAXNS];
      int nssocks[MAXNS];
      uint16_t nscount6;
      uint16_t nsinit;
      struct sockaddr_in6 *nsaddrs[MAXNS];
      unsigned int __glibc_reserved[2];
    } _ext;
  } _u;
};

#endif // LLVM_LIBC_TYPES_STRUCT___RES_STATE_H
