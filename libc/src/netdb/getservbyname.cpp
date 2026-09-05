//===-- Implementation of getservbyname -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/getservbyname.h"

#include "src/__support/common.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/netdb/resolv/services.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/string/string_utils.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// The answer is kept in storage the library owns, which the next lookup from
// this thread overwrites. That is what this interface has always been.
struct ServentStorage {
  struct servent entry;
  char name[64];
  char protocol[16];
  char *aliases[1];
};

LIBC_THREAD_LOCAL ServentStorage storage;

} // anonymous namespace

LLVM_LIBC_FUNCTION(struct servent *, getservbyname,
                   (const char *name, const char *proto)) {
  if (name == nullptr)
    return nullptr;

  uint16_t port = 0;
  // With no protocol asked for, the one the file gave is reported back.
  if (!resolv::port_for_service(name, proto, port, storage.protocol,
                                sizeof(storage.protocol)))
    return nullptr;

  const size_t length = internal::string_length(name);
  if (length >= sizeof(storage.name))
    return nullptr;
  inline_memcpy(storage.name, name, length + 1);

  storage.aliases[0] = nullptr;
  storage.entry.s_name = storage.name;
  storage.entry.s_aliases = storage.aliases;
  storage.entry.s_port = static_cast<int>(port);
  storage.entry.s_proto = storage.protocol;
  return &storage.entry;
}

} // namespace LIBC_NAMESPACE_DECL
