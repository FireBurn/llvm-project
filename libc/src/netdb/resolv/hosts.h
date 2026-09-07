//===-- Reading the hosts database ------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_RESOLV_HOSTS_H
#define LLVM_LIBC_SRC_NETDB_RESOLV_HOSTS_H

#include "hdr/types/size_t.h"
#include "hdr/types/struct_hostent.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

// Where an entry read out of the file and the strings it points at are kept.
// The interface hands back a pointer into storage the library owns, which the
// next read replaces, so there is one of these per thread.
struct HostsStorage {
  static constexpr size_t MAX_ALIASES = 15;
  static constexpr size_t POOL_SIZE = 512;
  static constexpr size_t ADDRESS_SIZE = 16;

  struct hostent entry;
  char *aliases[MAX_ALIASES + 1];
  // One address, then the terminator. A line of the file names one.
  char *address_list[2];
  unsigned char address[ADDRESS_SIZE];
  char pool[POOL_SIZE];
  size_t used;
};

// Reads the entries one after another. `rewind_hosts` starts again from the
// front, holding the file open between reads where asked; `stop_hosts`
// closes it; `next_host` fills `storage` from the line after the last one
// read and returns null at the end of the file.
void rewind_hosts(bool stay_open);
void stop_hosts();
struct hostent *next_host(HostsStorage &storage);

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_RESOLV_HOSTS_H
