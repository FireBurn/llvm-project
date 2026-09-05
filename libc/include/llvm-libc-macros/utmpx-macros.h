//===-- Macros defined in utmpx.h header file -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_UTMPX_MACROS_H
#define LLVM_LIBC_MACROS_UTMPX_MACROS_H

// The sizes of the three character fields of a record. <utmp.h> defines the
// same three, and a program may end up with both headers, so each is only
// defined once. glibc offers them from here as well, and code that includes
// <utmpx.h> alone counts on finding them.
#ifndef UT_LINESIZE
#define UT_LINESIZE 32
#endif
#ifndef UT_NAMESIZE
#define UT_NAMESIZE 32
#endif
#ifndef UT_HOSTSIZE
#define UT_HOSTSIZE 256
#endif

// What ut_type says a record is.
#define EMPTY 0         // Nothing here.
#define RUN_LVL 1       // The run level changed.
#define BOOT_TIME 2     // When the machine came up.
#define NEW_TIME 3      // The clock was set, and this is the time after.
#define OLD_TIME 4      // The clock was set, and this is the time before.
#define INIT_PROCESS 5  // A process init started.
#define LOGIN_PROCESS 6 // The leader of a session someone logged into.
#define USER_PROCESS 7  // An ordinary process.
#define DEAD_PROCESS 8  // One which has ended.
#define ACCOUNTING 9

#endif // LLVM_LIBC_MACROS_UTMPX_MACROS_H
