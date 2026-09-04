//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux specific declarations of macros from unistd.h.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_UNISTD_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_UNISTD_MACROS_H

// Values for mode argument to the access(...) function.
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4

// The numbers are the ones Linux already uses, so a program built against
// another libc and run here asks the same questions.
#define _SC_ARG_MAX 0
#define _SC_CHILD_MAX 1
#define _SC_CLK_TCK 2
#define _SC_NGROUPS_MAX 3
#define _SC_OPEN_MAX 4
#define _SC_STREAM_MAX 5
#define _SC_TZNAME_MAX 6
#define _SC_JOB_CONTROL 7
#define _SC_SAVED_IDS 8
#define _SC_VERSION 29
#define _SC_PAGESIZE 30
#define _SC_PAGE_SIZE _SC_PAGESIZE
#define _SC_LINE_MAX 43
#define _SC_RE_DUP_MAX 44
#define _SC_IOV_MAX 60
#define _SC_GETGR_R_SIZE_MAX 69
#define _SC_GETPW_R_SIZE_MAX 70
#define _SC_LOGIN_NAME_MAX 71
#define _SC_TTY_NAME_MAX 72
#define _SC_THREAD_STACK_MIN 75
#define _SC_THREAD_THREADS_MAX 76
#define _SC_THREAD_KEYS_MAX 74
#define _SC_ATEXIT_MAX 87
#define _SC_SYMLOOP_MAX 173
#define _SC_HOST_NAME_MAX 180
#define _SC_THREADS 67
#define _SC_NPROCESSORS_CONF 83
#define _SC_NPROCESSORS_ONLN 84
#define _SC_PHYS_PAGES 85
#define _SC_AVPHYS_PAGES 86

#define _PC_FILESIZEBITS 0
#define _PC_LINK_MAX 1
#define _PC_MAX_CANON 2
#define _PC_MAX_INPUT 3
#define _PC_NAME_MAX 4
#define _PC_PATH_MAX 5
#define _PC_PIPE_BUF 6
#define _PC_2_SYMLINKS 7
#define _PC_ALLOC_SIZE_MIN 8
#define _PC_REC_INCR_XFER_SIZE 9
#define _PC_REC_MAX_XFER_SIZE 10
#define _PC_REC_MIN_XFER_SIZE 11
#define _PC_REC_XFER_ALIGN 12
#define _PC_SYMLINK_MAX 13
#define _PC_CHOWN_RESTRICTED 14
#define _PC_NO_TRUNC 15
#define _PC_VDISABLE 16
#define _PC_ASYNC_IO 17
#define _PC_PRIO_IO 18
#define _PC_SYNC_IO 19

// TODO: Move these limit macros to a separate file
// The edition of POSIX these interfaces follow. This is what a program reads
// to find out it is on a POSIX system at all, and without it configure
// scripts fall back to pre-POSIX interfaces which are not here.
#define _POSIX_VERSION 202405L
#define _POSIX2_VERSION 202405L
#define _XOPEN_VERSION 800

// The options which are supported. Leaving one undefined means it has to be
// asked about at run time with sysconf; a value of -1 would mean it is not
// available at all.
#define _POSIX_CHOWN_RESTRICTED 1
#define _POSIX_THREADS 202405L
#define _POSIX_JOB_CONTROL 1
#define _POSIX_SAVED_IDS 1
#define _POSIX_THREAD_SAFE_FUNCTIONS 202405L
#define _POSIX_MAPPED_FILES 202405L
#define _POSIX_MEMORY_PROTECTION 202405L
#define _POSIX_FSYNC 202405L
#define _POSIX_SYNCHRONIZED_IO 202405L
#define _POSIX_TIMERS 202405L
#define _POSIX_MONOTONIC_CLOCK 202405L
#define _POSIX_CLOCK_SELECTION 202405L
#define _POSIX_SEMAPHORES 202405L
#define _POSIX_SHELL 1
#define _POSIX_PIPE_BUF 512
#define _POSIX_NO_TRUNC 1
#define _POSIX_VDISABLE '\0'

// Macro to set up the call to the __llvm_libc_syscall function
// This is to prevent the call from having fewer than 6 arguments, since six
// arguments are always passed to the syscall. Unnecessary arguments are
// ignored.
#define __syscall_helper(sysno, arg1, arg2, arg3, arg4, arg5, arg6, ...)       \
  __llvm_libc_syscall((long)(sysno), (long)(arg1), (long)(arg2), (long)(arg3), \
                      (long)(arg4), (long)(arg5), (long)(arg6))
#define syscall(...) __syscall_helper(__VA_ARGS__, 0, 1, 2, 3, 4, 5, 6)

#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression)                                         \
  (__extension__({                                                             \
    long __result;                                                             \
    do {                                                                       \
      __result = (long)(expression);                                           \
    } while (__result == -1L && errno == EINTR);                               \
    __result;                                                                  \
  }))
#endif

#endif // LLVM_LIBC_MACROS_LINUX_UNISTD_MACROS_H
