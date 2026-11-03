//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the copying of an entry of a netdb database into a buffer
/// of the caller's.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_REENTRANT_H
#define LLVM_LIBC_SRC_NETDB_REENTRANT_H

#include "hdr/types/size_t.h"
#include "hdr/types/struct_hostent.h"
#include "hdr/types/struct_netent.h"
#include "hdr/types/struct_protoent.h"
#include "hdr/types/struct_servent.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/string/string_utils.h"

namespace LIBC_NAMESPACE_DECL {
namespace netdb {

// Hands out pieces of a buffer the caller supplied, aligned for what they
// hold. A piece that does not fit is refused, which the reentrant functions
// report as ERANGE.
class Arena {
  char *next;
  size_t left;

public:
  LIBC_INLINE Arena(char *buffer, size_t size) : next(buffer), left(size) {}

  LIBC_INLINE void *take(size_t size, size_t alignment) {
    size_t skew = reinterpret_cast<size_t>(next) % alignment;
    size_t pad = skew == 0 ? 0 : alignment - skew;
    if (pad > left || size > left - pad)
      return nullptr;
    void *piece = next + pad;
    next += pad + size;
    left -= pad + size;
    return piece;
  }

  LIBC_INLINE char *copy(const char *text) {
    if (text == nullptr)
      return nullptr;
    size_t length = internal::string_length(text) + 1;
    char *piece = static_cast<char *>(take(length, 1));
    if (piece != nullptr)
      inline_memcpy(piece, text, length);
    return piece;
  }

  // A list of strings ending in a null pointer, which is how the entries keep
  // their aliases.
  LIBC_INLINE char **copy_names(char *const *names) {
    size_t count = 0;
    while (names != nullptr && names[count] != nullptr)
      ++count;
    auto **list = static_cast<char **>(
        take((count + 1) * sizeof(char *), alignof(char *)));
    if (list == nullptr)
      return nullptr;
    for (size_t i = 0; i < count; ++i) {
      list[i] = copy(names[i]);
      if (list[i] == nullptr)
        return nullptr;
    }
    list[count] = nullptr;
    return list;
  }

  // The addresses of a host: |count| of |size| bytes, each pointed to from
  // the list.
  LIBC_INLINE char **copy_addresses(char *const *addresses, size_t size) {
    size_t count = 0;
    while (addresses != nullptr && addresses[count] != nullptr)
      ++count;
    auto **list = static_cast<char **>(
        take((count + 1) * sizeof(char *), alignof(char *)));
    if (list == nullptr)
      return nullptr;
    for (size_t i = 0; i < count; ++i) {
      char *piece = static_cast<char *>(take(size, 1));
      if (piece == nullptr)
        return nullptr;
      inline_memcpy(piece, addresses[i], size);
      list[i] = piece;
    }
    list[count] = nullptr;
    return list;
  }
};

LIBC_INLINE bool copy_entry(Arena &arena, const struct hostent &from,
                            struct hostent *to) {
  to->h_addrtype = from.h_addrtype;
  to->h_length = from.h_length;
  to->h_name = arena.copy(from.h_name);
  to->h_aliases = arena.copy_names(from.h_aliases);
  to->h_addr_list = arena.copy_addresses(from.h_addr_list,
                                         static_cast<size_t>(from.h_length));
  return to->h_name != nullptr && to->h_aliases != nullptr &&
         to->h_addr_list != nullptr;
}

LIBC_INLINE bool copy_entry(Arena &arena, const struct servent &from,
                            struct servent *to) {
  to->s_port = from.s_port;
  to->s_name = arena.copy(from.s_name);
  to->s_aliases = arena.copy_names(from.s_aliases);
  to->s_proto = arena.copy(from.s_proto);
  return to->s_name != nullptr && to->s_aliases != nullptr &&
         to->s_proto != nullptr;
}

LIBC_INLINE bool copy_entry(Arena &arena, const struct protoent &from,
                            struct protoent *to) {
  to->p_proto = from.p_proto;
  to->p_name = arena.copy(from.p_name);
  to->p_aliases = arena.copy_names(from.p_aliases);
  return to->p_name != nullptr && to->p_aliases != nullptr;
}

LIBC_INLINE bool copy_entry(Arena &arena, const struct netent &from,
                            struct netent *to) {
  to->n_addrtype = from.n_addrtype;
  to->n_net = from.n_net;
  to->n_name = arena.copy(from.n_name);
  to->n_aliases = arena.copy_names(from.n_aliases);
  return to->n_name != nullptr && to->n_aliases != nullptr;
}

} // namespace netdb
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_REENTRANT_H
