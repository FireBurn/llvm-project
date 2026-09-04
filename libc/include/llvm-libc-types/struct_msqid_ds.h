//===-- Definition of struct msqid_ds -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_MSQID_DS_H
#define LLVM_LIBC_TYPES_STRUCT_MSQID_DS_H

#include "pid_t.h"
#include "size_t.h"
#include "struct_ipc_perm.h"
#include "time_t.h"

// What msgctl reports about a message queue. The layout is the kernel's, so
// the unused members are the padding it keeps for the times on the targets
// where they are wider than a long.
struct msqid_ds {
  struct ipc_perm msg_perm;
  time_t msg_stime; // When a message was last sent.
#if defined(__linux__) && __SIZEOF_LONG__ == 4
  // Where a long is too narrow to hold the time, the kernel keeps the top
  // half of each one in a word of its own after it.
  unsigned long __msg_stime_high;
#endif
  time_t msg_rtime; // When a message was last received.
#if defined(__linux__) && __SIZEOF_LONG__ == 4
  unsigned long __msg_rtime_high;
#endif
  time_t msg_ctime; // When the queue was last changed.
#if defined(__linux__) && __SIZEOF_LONG__ == 4
  unsigned long __msg_ctime_high;
#endif
  unsigned long __msg_cbytes; // How many bytes are on the queue now.
  size_t msg_qnum;            // How many messages are on it.
  size_t msg_qbytes;          // The most bytes it will hold.
  pid_t msg_lspid;            // Who sent the last message.
  pid_t msg_lrpid;            // Who received it.
#ifdef __linux__
  unsigned long __unused4;
  unsigned long __unused5;
#endif
};

#endif // LLVM_LIBC_TYPES_STRUCT_MSQID_DS_H
