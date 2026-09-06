//===-- Reading the protocols database --------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_RESOLV_PROTOCOLS_H
#define LLVM_LIBC_SRC_NETDB_RESOLV_PROTOCOLS_H

#include "hdr/types/size_t.h"
#include "hdr/types/struct_protoent.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

// Where the entries and the strings they point at are kept. The interface
// hands back a pointer into storage the library owns, which the next lookup
// replaces, so there is one of these per thread.
struct ProtoStorage {
  static constexpr size_t MAX_ALIASES = 15;
  static constexpr size_t POOL_SIZE = 256;

  struct protoent entry;
  char *aliases[MAX_ALIASES + 1];
  char pool[POOL_SIZE];
  size_t used;
};

// Fills `storage` from the first line naming `name`, either as the official
// name or as one of the other names. Returns null where the file says
// nothing about it.
struct protoent *by_name(const char *name, ProtoStorage &storage);

// The same, for the number a protocol is known by in an IP header.
struct protoent *by_number(int number, ProtoStorage &storage);

// Reads the entries one after another. `rewind_entries` starts again from the
// front, holding the file open between reads where asked; `stop_entries`
// closes it; `next_entry` fills `storage` from the line after the last one
// read and returns null at the end of the file.
void rewind_entries(bool stay_open);
void stop_entries();
struct protoent *next_entry(ProtoStorage &storage);

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_RESOLV_PROTOCOLS_H
