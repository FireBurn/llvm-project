//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_TMS_H
#define LLVM_LIBC_TYPES_STRUCT_TMS_H

#include "clock_t.h"

/// How much processor time has been used, counted in clock ticks.
struct tms {
  /// Time this process spent running its own code.
  clock_t tms_utime;
  /// Time the system spent on its behalf.
  clock_t tms_stime;
  /// The same two for the children it has waited for, and for their children
  /// in turn.
  clock_t tms_cutime;
  clock_t tms_cstime;
};

#endif // LLVM_LIBC_TYPES_STRUCT_TMS_H
