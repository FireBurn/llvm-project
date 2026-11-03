//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the netgroup database shared by its functions.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_NETGROUP_UTILS_H
#define LLVM_LIBC_SRC_NETDB_NETGROUP_UTILS_H

#include "hdr/types/size_t.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace netgroup {

// A field of a triple which is absent is a null pointer, and matches
// anything.
struct Triple {
  const char *host;
  const char *user;
  const char *domain;
};

// The triples of one netgroup, with the groups it names expanded into them.
// The strings are kept in one block, so a Triple stays valid until the group
// is released.
struct Group {
  char *text;
  size_t text_used;
  size_t text_capacity;
  // Three offsets into the text for each triple.
  size_t *offsets;
  size_t count;
  size_t capacity;
  size_t cursor;
};

// Fills `group` with the triples of `name` from the sources nsswitch.conf
// names for netgroup. Returns false if no source has it.
bool load(cpp::string_view name, Group &group);

// The next triple, or false after the last.
bool next(Group &group, Triple &triple);

void release(Group &group);

// The netgroup the calling thread is reading with getnetgrent.
Group &current();

// Whether any triple of the group matches. A null argument matches any triple.
bool contains(const Group &group, const char *host, const char *user,
              const char *domain);

void TESTONLY_set_netgroup_path(const char *path);
void TESTONLY_reset_netgroup_path();

} // namespace netgroup
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_NETGROUP_UTILS_H
