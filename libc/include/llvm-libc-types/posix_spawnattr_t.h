//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of type posix_spawnattr_t.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_POSIX_SPAWNATTR_T_H
#define LLVM_LIBC_TYPES_POSIX_SPAWNATTR_T_H

#include "pid_t.h"
#include "sigset_t.h"
#include "struct_sched_param.h"

// What the child is to have set up before it is run. Which of these are
// looked at is decided by __flags; the rest are left alone.
typedef struct {
  short __flags;
  pid_t __pgroup;                  // POSIX_SPAWN_SETPGROUP
  sigset_t __sigdefault;           // POSIX_SPAWN_SETSIGDEF
  sigset_t __sigmask;              // POSIX_SPAWN_SETSIGMASK
  int __policy;                    // POSIX_SPAWN_SETSCHEDULER
  struct sched_param __schedparam; // POSIX_SPAWN_SETSCHEDPARAM
} posix_spawnattr_t;

#endif // LLVM_LIBC_TYPES_POSIX_SPAWNATTR_T_H
