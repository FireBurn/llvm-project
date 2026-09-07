//===-- Definition of socklen_t type ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_SOCKLEN_T_H
#define LLVM_LIBC_TYPES_SOCKLEN_T_H

// POSIX asks only for an integer type at least 32 bits wide, but the width is
// not free to choose: socklen_t sits in structures the kernel reads and is
// pointed at by arguments the kernel writes through, and it appears in the
// interfaces of every other library a program is linked with. Everywhere this
// runs, that type is 32 bits wide.

typedef unsigned int socklen_t;

#endif // LLVM_LIBC_TYPES_SOCKLEN_T_H
