//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SERVENT_H
#define LLVM_LIBC_TYPES_STRUCT_SERVENT_H

/// What is known about a service.
struct servent {
  /// The service's official name.
  char *s_name;
  /// The other names it goes by, ending in a null pointer.
  char **s_aliases;
  /// The port it is registered on, in the order the wire uses.
  int s_port;
  /// The protocol it is carried over.
  char *s_proto;
};

#endif // LLVM_LIBC_TYPES_STRUCT_SERVENT_H
