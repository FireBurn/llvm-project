//===-- Definition of type struct timezone --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_TIMEZONE_H
#define LLVM_LIBC_TYPES_STRUCT_TIMEZONE_H

// The kernel stopped keeping a timezone a long time ago, so nothing fills
// this in and nothing reads it. It is here because the calls which used to
// take one still name it in their prototypes.
struct timezone {
  int tz_minuteswest; // Minutes west of Greenwich.
  int tz_dsttime;     // The kind of daylight saving correction.
};

#endif // LLVM_LIBC_TYPES_STRUCT_TIMEZONE_H
