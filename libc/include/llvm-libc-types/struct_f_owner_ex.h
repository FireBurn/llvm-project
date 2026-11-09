//===-- Definition of type struct f_owner_ex ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_F_OWNER_EX_H
#define LLVM_LIBC_TYPES_STRUCT_F_OWNER_EX_H

#include "pid_t.h"

// <linux/fcntl.h> defines the same struct f_owner_ex and the F_OWNER_* macros.
// A program that includes it first, as Mesa and others do, would see a
// redefinition, so libc defers to it then.
#ifndef _ASM_GENERIC_FCNTL_H
enum __pid_type {
  F_OWNER_TID = 0,
  F_OWNER_PID,
  F_OWNER_PGRP,
};

struct f_owner_ex {
  enum __pid_type type;
  pid_t pid;
};
#endif // _ASM_GENERIC_FCNTL_H

#endif // LLVM_LIBC_TYPES_STRUCT_F_OWNER_EX_H
