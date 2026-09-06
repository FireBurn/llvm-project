//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_PROTOENT_H
#define LLVM_LIBC_TYPES_STRUCT_PROTOENT_H

/// What is known about a protocol carried over IP.
struct protoent {
  /// The protocol's official name.
  char *p_name;
  /// The other names it goes by, ending in a null pointer.
  char **p_aliases;
  /// The number that names it in an IP header.
  int p_proto;
};

#endif // LLVM_LIBC_TYPES_STRUCT_PROTOENT_H
