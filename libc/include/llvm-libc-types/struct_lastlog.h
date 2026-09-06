//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct lastlog.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_LASTLOG_H
#define LLVM_LIBC_TYPES_STRUCT_LASTLOG_H

#include "../llvm-libc-macros/stdint-macros.h"

// One entry of /var/log/lastlog, which is indexed by user id rather than
// read in order. The time is the width it is on a 32 bit machine, because
// the file is shared between 32 and 64 bit programs.
struct lastlog {
  int32_t ll_time;
  char ll_line[32];
  char ll_host[256];
};

#endif // LLVM_LIBC_TYPES_STRUCT_LASTLOG_H
