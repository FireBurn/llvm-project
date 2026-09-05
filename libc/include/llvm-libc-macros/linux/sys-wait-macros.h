//===-- Definition of macros from sys/wait.h ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_SYS_WAIT_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_SYS_WAIT_MACROS_H

// What a wait call is willing to be told about, and whether it should
// block. These are the kernel's numbers, spelled out rather than taken from
// <linux/wait.h>, which also defines the P_* names that idtype_t's
// enumerators carry.
#define WNOHANG 0x00000001
#define WUNTRACED 0x00000002
#define WSTOPPED WUNTRACED
#define WEXITED 0x00000004
#define WCONTINUED 0x00000008
#define WNOWAIT 0x01000000

#define __WNOTHREAD 0x20000000
#define __WALL 0x40000000
#define __WCLONE 0x80000000

#define WCOREDUMP(status) ((status) & WCOREFLAG)
#define WEXITSTATUS(status) (((status) & 0xff00) >> 8)
#define WIFCONTINUED(status) ((status) == 0xffff)
#define WIFEXITED(status) (WTERMSIG(status) == 0)
#define WIFSIGNALED(status) ((WTERMSIG(status) + 1) >= 2)
#define WIFSTOPPED(status) (WTERMSIG(status) == 0x7f)
#define WSTOPSIG(status) WEXITSTATUS(status)
#define WTERMSIG(status) ((status) & 0x7f)

#define WCOREFLAG 0x80
#define W_EXITCODE(ret, sig) ((ret) << 8 | (sig))
#define W_STOPCODE(sig) ((sig) << 8 | 0x7f)

#endif // LLVM_LIBC_MACROS_LINUX_SYS_WAIT_MACROS_H
