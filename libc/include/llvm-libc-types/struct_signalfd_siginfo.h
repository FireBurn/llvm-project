//===-- Definition of struct signalfd_siginfo -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SIGNALFD_SIGINFO_H
#define LLVM_LIBC_TYPES_STRUCT_SIGNALFD_SIGINFO_H

#include "../llvm-libc-macros/stdint-macros.h"

// What a read from a signalfd hands back: one of these for each signal
// which arrived. The layout is the kernel's, and the padding at the end
// keeps it at 128 bytes so that a future member does not move the others.
struct signalfd_siginfo {
  uint32_t ssi_signo; // Which signal it was.
  int32_t ssi_errno;
  int32_t ssi_code; // Why it was sent.
  uint32_t ssi_pid; // Who sent it.
  uint32_t ssi_uid;
  int32_t ssi_fd;   // The descriptor, for SIGIO.
  uint32_t ssi_tid; // The timer, for a timer signal.
  uint32_t ssi_band;
  uint32_t ssi_overrun; // How many times that timer had already fired.
  uint32_t ssi_trapno;
  int32_t ssi_status; // The exit status, for SIGCHLD.
  int32_t ssi_int;    // What sigqueue was given to send.
  uint64_t ssi_ptr;
  uint64_t ssi_utime; // The child's time, for SIGCHLD.
  uint64_t ssi_stime;
  uint64_t ssi_addr; // The address, for a fault.
  uint16_t ssi_addr_lsb;
  uint16_t __pad2;
  int32_t ssi_syscall;
  uint64_t ssi_call_addr;
  uint32_t ssi_arch;
  uint8_t __pad[28];
};

#endif // LLVM_LIBC_TYPES_STRUCT_SIGNALFD_SIGINFO_H
