//===-- Macros defined in sys/xattr.h header file -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_SYS_XATTR_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_SYS_XATTR_MACROS_H

// Whether an attribute which is already there may be replaced, and whether
// one which is not there may be made.
#define XATTR_CREATE 1  // Fail if the attribute is already there.
#define XATTR_REPLACE 2 // Fail if it is not.

#endif // LLVM_LIBC_MACROS_LINUX_SYS_XATTR_MACROS_H
