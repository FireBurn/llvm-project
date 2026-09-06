//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct utmp.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_UTMP_H
#define LLVM_LIBC_TYPES_STRUCT_UTMP_H

#include "../llvm-libc-macros/stdint-macros.h"
#include "pid_t.h"

#define __UT_LINESIZE 32
#define __UT_NAMESIZE 32
#define __UT_HOSTSIZE 256

// How a process which was marked dead ended.
struct exit_status {
  short int e_termination;
  short int e_exit;
};

// The same record as struct utmpx, under the name the traditional interface
// uses. The two are one file format and one set of calls; only the names
// differ, POSIX having standardised the x forms of both.
struct utmp {
  short int ut_type;           // What kind of record this is.
  pid_t ut_pid;                // The process it is about.
  char ut_line[__UT_LINESIZE]; // The terminal.
  char ut_id[4];               // The inittab entry.
  char ut_user[__UT_NAMESIZE]; // Who.
  char ut_host[__UT_HOSTSIZE]; // Where from, for a remote login.
  struct exit_status ut_exit;  // How it ended, for a dead process.
  int32_t ut_session;          // The session, for a windowing system.
  struct {
    int32_t tv_sec;
    int32_t tv_usec;
  } ut_tv;               // When the record was made.
  int32_t ut_addr_v6[4]; // The remote address.
  char __libc_reserved[20];
};

// The older names for two of the fields, which some code still uses.
#define ut_name ut_user
#define ut_time ut_tv.tv_sec
#define ut_addr ut_addr_v6[0]

#endif // LLVM_LIBC_TYPES_STRUCT_UTMP_H
