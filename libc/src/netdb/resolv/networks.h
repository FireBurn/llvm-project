//===-- Reading the networks database ---------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_RESOLV_NETWORKS_H
#define LLVM_LIBC_SRC_NETDB_RESOLV_NETWORKS_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_netent.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

// Where the entry and the strings it points at are kept, one per thread,
// since each lookup replaces what the last handed back.
struct NetStorage {
  static constexpr size_t MAX_ALIASES = 15;
  static constexpr size_t POOL_SIZE = 256;

  struct netent entry;
  char *aliases[MAX_ALIASES + 1];
  char pool[POOL_SIZE];
  size_t used;
};

// Fills `storage` from the line naming `name`, or from the one whose number
// is `net`. Returns null where the file says nothing about it.
struct netent *net_by_name(const char *name, NetStorage &storage);
struct netent *net_by_addr(uint32_t net, int type, NetStorage &storage);

// Reads the entries one after another.
void rewind_networks(bool stay_open);
void stop_networks();
struct netent *next_network(NetStorage &storage);

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_RESOLV_NETWORKS_H
