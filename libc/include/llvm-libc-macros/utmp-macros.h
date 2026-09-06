//===-- Macros defined in utmp.h header file ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_UTMP_MACROS_H
#define LLVM_LIBC_MACROS_UTMP_MACROS_H

// The sizes of the three character fields of a record. <utmpx.h> defines the
// same three, and a program may end up with both headers.
#ifndef UT_LINESIZE
#define UT_LINESIZE 32
#endif
#ifndef UT_NAMESIZE
#define UT_NAMESIZE 32
#endif
#ifndef UT_HOSTSIZE
#define UT_HOSTSIZE 256
#endif

// What ut_type says a record is. The same set <utmpx.h> defines, and the
// same file, so a program may end up with both headers.
#ifndef EMPTY
#define EMPTY 0 // Nothing here.
#endif
#ifndef RUN_LVL
#define RUN_LVL 1 // The run level changed.
#endif
#ifndef BOOT_TIME
#define BOOT_TIME 2 // When the machine came up.
#endif
#ifndef NEW_TIME
#define NEW_TIME 3 // The clock was set, and this is the time after.
#endif
#ifndef OLD_TIME
#define OLD_TIME 4 // The clock was set, and this is the time before.
#endif
#ifndef INIT_PROCESS
#define INIT_PROCESS 5 // A process init started.
#endif
#ifndef LOGIN_PROCESS
#define LOGIN_PROCESS 6 // The leader of a session someone logged into.
#endif
#ifndef USER_PROCESS
#define USER_PROCESS 7 // An ordinary process.
#endif
#ifndef DEAD_PROCESS
#define DEAD_PROCESS 8 // One which has ended.
#endif
#ifndef ACCOUNTING
#define ACCOUNTING 9
#endif

// Where the files live. A program which wants a file of its own passes it
// to utmpname instead.
#define _PATH_UTMP "/var/run/utmp"
#define _PATH_WTMP "/var/log/wtmp"
#define _PATH_LASTLOG "/var/log/lastlog"
#define UTMP_FILE _PATH_UTMP
#define UTMP_FILENAME _PATH_UTMP
#define WTMP_FILE _PATH_WTMP
#define WTMP_FILENAME _PATH_WTMP

#endif // LLVM_LIBC_MACROS_UTMP_MACROS_H
