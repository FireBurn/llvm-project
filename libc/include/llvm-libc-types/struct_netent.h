//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct netent.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_NETENT_H
#define LLVM_LIBC_TYPES_STRUCT_NETENT_H

#include "../llvm-libc-macros/stdint-macros.h"

/// What is known about a network. The number is held in one word, which is
/// what the interface has always assumed and is why it names IPv4 networks
/// only.
struct netent {
  /// The network's official name.
  char *n_name;
  /// The other names it goes by, ending in a null pointer.
  char **n_aliases;
  /// Which kind of address the number is, which is always AF_INET.
  int n_addrtype;
  /// The network number, in the order the host uses.
  uint32_t n_net;
};

#endif // LLVM_LIBC_TYPES_STRUCT_NETENT_H
