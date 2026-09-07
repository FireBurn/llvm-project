//===-- Definition of the access mode macros ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_ACCESS_MODE_MACROS_H
#define LLVM_LIBC_MACROS_ACCESS_MODE_MACROS_H

// What access and faccessat are being asked about. The first three may be
// combined; F_OK asks only whether the file is there at all.
//
// These belong to <unistd.h>, where the calls that take them are declared,
// but <fcntl.h> hands them over too. It is where AT_EACCESS is, which says
// which user the question is asked as and means nothing without them, and it
// is where every other library puts them.
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4

#endif // LLVM_LIBC_MACROS_ACCESS_MODE_MACROS_H
