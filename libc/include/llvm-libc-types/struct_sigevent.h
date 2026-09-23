//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct sigevent type.
/// https://man7.org/linux/man-pages/man3/sigevent.3type.html
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SIGEVENT_H
#define LLVM_LIBC_TYPES_STRUCT_SIGEVENT_H

#include "pid_t.h"
#include "pthread_attr_t.h"
#include "union_sigval.h"

// The kernel's layout, which glibc and musl use too, with the members POSIX
// names reached through macros as glibc has them.
struct sigevent {
  union sigval sigev_value;
  int sigev_signo;
  int sigev_notify;
  union {
    int _pad[12];
    pid_t _tid;
    struct {
      void (*_function)(union sigval);
      pthread_attr_t *_attribute;
    } _sigev_thread;
  } _sigev_un;
};

#define sigev_notify_function _sigev_un._sigev_thread._function
#define sigev_notify_attributes _sigev_un._sigev_thread._attribute
#define sigev_notify_thread_id _sigev_un._tid

#endif // LLVM_LIBC_TYPES_STRUCT_SIGEVENT_H
