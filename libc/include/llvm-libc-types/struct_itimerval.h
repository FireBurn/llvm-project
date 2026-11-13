//===-- Definition of struct itimerval ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_ITIMERVAL_H
#define LLVM_LIBC_TYPES_STRUCT_ITIMERVAL_H

#include "struct_timeval.h"

#ifdef __ANDROID__
// Android code gets this from <linux/time.h> as well, which defines it
// unguarded, so there the kernel's definition stands.
#include <linux/time.h>
#else
struct itimerval {
  struct timeval it_interval; /* Interval for periodic timer */
  struct timeval it_value;    /* Time until next expiration */
};
#endif // __ANDROID__

#endif // LLVM_LIBC_TYPES_STRUCT_ITIMERVAL_H
