//===-- Definition of struct timeb ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_TIMEB_H
#define LLVM_LIBC_TYPES_STRUCT_TIMEB_H

#include "time_t.h"

struct timeb {
  time_t time;
  unsigned short millitm;
  short timezone;
  short dstflag;
};

#endif // LLVM_LIBC_TYPES_STRUCT_TIMEB_H
