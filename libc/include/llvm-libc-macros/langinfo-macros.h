//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of macros from langinfo.h.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LANGINFO_MACROS_H
#define LLVM_LIBC_MACROS_LANGINFO_MACROS_H

#include "locale-macros.h"

#define _NL_ITEM(category, index) (((category) << 16) | (index))

// The indices glibc gives the items, so that the values are the same.
#define CODESET _NL_ITEM(LC_CTYPE, 14)
#define RADIXCHAR _NL_ITEM(LC_NUMERIC, 0)
#define THOUSEP _NL_ITEM(LC_NUMERIC, 1)
#define ABDAY_1 _NL_ITEM(LC_TIME, 0)
#define ABDAY_2 _NL_ITEM(LC_TIME, 1)
#define ABDAY_3 _NL_ITEM(LC_TIME, 2)
#define ABDAY_4 _NL_ITEM(LC_TIME, 3)
#define ABDAY_5 _NL_ITEM(LC_TIME, 4)
#define ABDAY_6 _NL_ITEM(LC_TIME, 5)
#define ABDAY_7 _NL_ITEM(LC_TIME, 6)
#define DAY_1 _NL_ITEM(LC_TIME, 7)
#define DAY_2 _NL_ITEM(LC_TIME, 8)
#define DAY_3 _NL_ITEM(LC_TIME, 9)
#define DAY_4 _NL_ITEM(LC_TIME, 10)
#define DAY_5 _NL_ITEM(LC_TIME, 11)
#define DAY_6 _NL_ITEM(LC_TIME, 12)
#define DAY_7 _NL_ITEM(LC_TIME, 13)
#define ABMON_1 _NL_ITEM(LC_TIME, 14)
#define ABMON_2 _NL_ITEM(LC_TIME, 15)
#define ABMON_3 _NL_ITEM(LC_TIME, 16)
#define ABMON_4 _NL_ITEM(LC_TIME, 17)
#define ABMON_5 _NL_ITEM(LC_TIME, 18)
#define ABMON_6 _NL_ITEM(LC_TIME, 19)
#define ABMON_7 _NL_ITEM(LC_TIME, 20)
#define ABMON_8 _NL_ITEM(LC_TIME, 21)
#define ABMON_9 _NL_ITEM(LC_TIME, 22)
#define ABMON_10 _NL_ITEM(LC_TIME, 23)
#define ABMON_11 _NL_ITEM(LC_TIME, 24)
#define ABMON_12 _NL_ITEM(LC_TIME, 25)
#define MON_1 _NL_ITEM(LC_TIME, 26)
#define MON_2 _NL_ITEM(LC_TIME, 27)
#define MON_3 _NL_ITEM(LC_TIME, 28)
#define MON_4 _NL_ITEM(LC_TIME, 29)
#define MON_5 _NL_ITEM(LC_TIME, 30)
#define MON_6 _NL_ITEM(LC_TIME, 31)
#define MON_7 _NL_ITEM(LC_TIME, 32)
#define MON_8 _NL_ITEM(LC_TIME, 33)
#define MON_9 _NL_ITEM(LC_TIME, 34)
#define MON_10 _NL_ITEM(LC_TIME, 35)
#define MON_11 _NL_ITEM(LC_TIME, 36)
#define MON_12 _NL_ITEM(LC_TIME, 37)
#define AM_STR _NL_ITEM(LC_TIME, 38)
#define PM_STR _NL_ITEM(LC_TIME, 39)
#define D_T_FMT _NL_ITEM(LC_TIME, 40)
#define D_FMT _NL_ITEM(LC_TIME, 41)
#define T_FMT _NL_ITEM(LC_TIME, 42)
#define T_FMT_AMPM _NL_ITEM(LC_TIME, 43)
#define ERA _NL_ITEM(LC_TIME, 44)
#define ERA_D_FMT _NL_ITEM(LC_TIME, 46)
#define ALT_DIGITS _NL_ITEM(LC_TIME, 47)
#define ERA_D_T_FMT _NL_ITEM(LC_TIME, 48)
#define ERA_T_FMT _NL_ITEM(LC_TIME, 49)
#define CRNCYSTR _NL_ITEM(LC_MONETARY, 15)
#define YESEXPR _NL_ITEM(LC_MESSAGES, 0)
#define NOEXPR _NL_ITEM(LC_MESSAGES, 1)

#endif // LLVM_LIBC_MACROS_LANGINFO_MACROS_H
