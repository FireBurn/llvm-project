//===-- Types the capability syscalls take ----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_CAPABILITY_CAP_TYPES_H
#define LLVM_LIBC_SRC_SYS_CAPABILITY_CAP_TYPES_H

#include "hdr/stdint_proxy.h"

// These are the shapes the kernel reads and writes. There is no header of
// our own for them: libcap owns <sys/capability.h> and declares the two
// calls itself, so a header here would collide with it. The declarations
// below match what libcap and the kernel expect.
typedef struct __user_cap_header_struct {
  uint32_t version;
  int pid;
} *cap_user_header_t;

struct __user_cap_data_struct {
  uint32_t effective;
  uint32_t permitted;
  uint32_t inheritable;
};
typedef struct __user_cap_data_struct *cap_user_data_t;

#endif // LLVM_LIBC_SRC_SYS_CAPABILITY_CAP_TYPES_H
