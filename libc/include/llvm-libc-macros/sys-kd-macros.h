//===-- Macros defined in sys/kd.h header file ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SYS_KD_MACROS_H
#define LLVM_LIBC_MACROS_SYS_KD_MACROS_H

// The console requests and the structures they carry are the kernel's, and
// are the same whatever names a libc gives them, so they are taken from where
// they are defined rather than written out again.
#include <linux/kd.h>

#endif // LLVM_LIBC_MACROS_SYS_KD_MACROS_H
