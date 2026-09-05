//===-- Where the older host lookups keep their answer ----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_HOSTENT_STORAGE_H
#define LLVM_LIBC_SRC_NETDB_HOSTENT_STORAGE_H

#include "hdr/types/struct_hostent.h"
#include "src/__support/macros/config.h"
#include "src/netdb/resolv/lookup.h"

namespace LIBC_NAMESPACE_DECL {
namespace netdb {

// These lookups hand back a pointer to storage the library owns, which the
// next call may overwrite. It is per thread, so two threads looking things up
// at once do not overwrite each other, which is as much as can be done for an
// interface whose whole shape is a shared answer.
struct HostentStorage {
  struct hostent entry;
  char name[resolv::MAX_NAME + 1];
  char *aliases[1];
  // A pointer to each address, then the terminator.
  char *address_list[resolv::MAX_ADDRESSES + 1];
  unsigned char addresses[resolv::MAX_ADDRESSES][16];
};

// The calling thread's storage.
HostentStorage &hostent_storage();

// Looks `name` up and fills the calling thread's storage in. Returns null and
// sets h_errno on failure.
struct hostent *fill_hostent(const char *name, int family);

} // namespace netdb
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_HOSTENT_STORAGE_H
