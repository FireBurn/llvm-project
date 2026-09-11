//===-- Macros defined in sys/un.h header file ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SYS_UN_MACROS_H
#define LLVM_LIBC_MACROS_SYS_UN_MACROS_H

// The length of an AF_UNIX address up to the end of its path, which is what
// bind and connect are given for a path shorter than sun_path. The builtins
// keep <stddef.h> and <string.h> out of <sys/un.h>.
#define SUN_LEN(ptr)                                                           \
  (__builtin_offsetof(struct sockaddr_un, sun_path) +                          \
   __builtin_strlen((ptr)->sun_path))

#endif // LLVM_LIBC_MACROS_SYS_UN_MACROS_H
