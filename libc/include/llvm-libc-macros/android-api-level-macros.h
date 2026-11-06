//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Macros defined in android/api-level.h header file.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_ANDROID_API_LEVEL_MACROS_H
#define LLVM_LIBC_MACROS_ANDROID_API_LEVEL_MACROS_H

#define __ANDROID_API_FUTURE__ 10000

// The level a program is built for. Everything this libc provides exists at
// every level, so the default is the newest.
#ifndef __ANDROID_API__
#define __ANDROID_API__ __ANDROID_API_FUTURE__
#endif

#define __ANDROID_API_G__ 9
#define __ANDROID_API_I__ 14
#define __ANDROID_API_J__ 16
#define __ANDROID_API_J_MR1__ 17
#define __ANDROID_API_J_MR2__ 18
#define __ANDROID_API_K__ 19
#define __ANDROID_API_L__ 21
#define __ANDROID_API_L_MR1__ 22
#define __ANDROID_API_M__ 23
#define __ANDROID_API_N__ 24
#define __ANDROID_API_N_MR1__ 25
#define __ANDROID_API_O__ 26
#define __ANDROID_API_O_MR1__ 27
#define __ANDROID_API_P__ 28
#define __ANDROID_API_Q__ 29
#define __ANDROID_API_R__ 30
#define __ANDROID_API_S__ 31
#define __ANDROID_API_T__ 33
#define __ANDROID_API_U__ 34
#define __ANDROID_API_V__ 35

#endif // LLVM_LIBC_MACROS_ANDROID_API_LEVEL_MACROS_H
