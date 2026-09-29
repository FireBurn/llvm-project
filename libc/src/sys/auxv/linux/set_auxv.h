//===-- Handing libc the auxiliary vector -----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_AUXV_LINUX_SET_AUXV_H
#define LLVM_LIBC_SRC_SYS_AUXV_LINUX_SET_AUXV_H

#include "src/__support/macros/config.h"

// Points libc.so at the auxiliary vector on the stack the program was started
// with. The startup code keeps its own copy of the pointer, and without this
// libc.so asks the kernel instead, which gives the vector the kernel first
// passed. That is a different one when something ran before the program and
// rewrote it, as Wine's preloader does.
extern "C" LIBC_SHARED_INTERNAL void __llvm_libc_set_auxv(const void *auxv);

#endif // LLVM_LIBC_SRC_SYS_AUXV_LINUX_SET_AUXV_H
