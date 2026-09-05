//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_HOSTENT_H
#define LLVM_LIBC_TYPES_STRUCT_HOSTENT_H

/// What is known about a host, as the older lookups report it.
struct hostent {
  /// The host's official name.
  char *h_name;
  /// The other names it goes by, ending in a null pointer.
  char **h_aliases;
  /// Which kind of address the list holds.
  int h_addrtype;
  /// How long each of them is.
  int h_length;
  /// The addresses, ending in a null pointer.
  char **h_addr_list;
};

/// What the first address used to be called, before there could be more
/// than one.
#define h_addr h_addr_list[0]

#endif // LLVM_LIBC_TYPES_STRUCT_HOSTENT_H
