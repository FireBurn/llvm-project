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

#include <linux/close_range.h>

// Whence values for lseek beyond the three the standard defines. These find
// where a sparse file's data and its holes begin.
#define SEEK_DATA 3
#define SEEK_HOLE 4

// Values for mode argument to the access(...) function.
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4

// Values for command argument to the lockf(...) function.
#define F_ULOCK 0
#define F_LOCK 1
#define F_TLOCK 2
#define F_TEST 3

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
#define _SC_AIO_LISTIO_MAX 23
#define _SC_AIO_MAX 24
#define _SC_AIO_PRIO_DELTA_MAX 25
#define _SC_MQ_OPEN_MAX 27
#define _SC_RTSIG_MAX 31
#define _SC_SEM_NSEMS_MAX 32
#define _SC_SIGQUEUE_MAX 34
#define _SC_TIMER_MAX 35
#define _SC_BC_BASE_MAX 36
#define _SC_BC_DIM_MAX 37
#define _SC_BC_SCALE_MAX 38
#define _SC_BC_STRING_MAX 39
#define _SC_COLL_WEIGHTS_MAX 40
#define _SC_EXPR_NEST_MAX 42
#define _SC_LEVEL1_ICACHE_SIZE 185
#define _SC_LEVEL1_ICACHE_ASSOC 186
#define _SC_LEVEL1_ICACHE_LINESIZE 187
#define _SC_LEVEL1_DCACHE_SIZE 188
#define _SC_LEVEL1_DCACHE_ASSOC 189
#define _SC_LEVEL1_DCACHE_LINESIZE 190
#define _SC_LEVEL2_CACHE_SIZE 191
#define _SC_LEVEL2_CACHE_ASSOC 192
#define _SC_LEVEL2_CACHE_LINESIZE 193
#define _SC_LEVEL3_CACHE_SIZE 194
#define _SC_LEVEL3_CACHE_ASSOC 195
#define _SC_LEVEL3_CACHE_LINESIZE 196
#define _SC_LEVEL4_CACHE_SIZE 197
#define _SC_LEVEL4_CACHE_ASSOC 198
#define _SC_LEVEL4_CACHE_LINESIZE 199
#define _SC_GETGR_R_SIZE_MAX 69
#define _SC_GETPW_R_SIZE_MAX 70
#define _SC_THREAD_DESTRUCTOR_ITERATIONS 73
#define _SC_LOGIN_NAME_MAX 71
#define _SC_TTY_NAME_MAX 72
#define _SC_THREAD_STACK_MIN 75
#define _SC_THREAD_THREADS_MAX 76
#define _SC_THREAD_KEYS_MAX 74
#define _SC_ATEXIT_MAX 87
#define _SC_SYMLOOP_MAX 173
#define _SC_HOST_NAME_MAX 180
#define _SC_THREADS 67
#define _SC_THREAD_SAFE_FUNCTIONS 68
#define _SC_THREAD_ATTR_STACKADDR 77
#define _SC_THREAD_ATTR_STACKSIZE 78
#define _SC_THREAD_PRIORITY_SCHEDULING 79
#define _SC_THREAD_PRIO_INHERIT 80
#define _SC_THREAD_PRIO_PROTECT 81
#define _SC_THREAD_PROCESS_SHARED 82
#define _SC_NPROCESSORS_CONF 83
#define _SC_NPROCESSORS_ONLN 84
#define _SC_PHYS_PAGES 85
#define _SC_AVPHYS_PAGES 86
#define _SC_THREAD_CPUTIME 139
#define _SC_THREAD_SPORADIC_SERVER 161
#define _SC_THREAD_ROBUST_PRIO_INHERIT 247
#define _SC_THREAD_ROBUST_PRIO_PROTECT 248
#define _SC_TIMERS 11
#define _SC_SYNCHRONIZED_IO 14
#define _SC_FSYNC 15
#define _SC_MAPPED_FILES 16
#define _SC_MEMORY_PROTECTION 19
#define _SC_SEMAPHORES 21
#define _SC_DELAYTIMER_MAX 26
#define _SC_MQ_PRIO_MAX 28
#define _SC_SEM_VALUE_MAX 33
#define _SC_2_VERSION 46
#define _SC_XOPEN_VERSION 89
#define _SC_UIO_MAXIOV _SC_IOV_MAX
#define _SC_XOPEN_UNIX 91
#define _SC_XOPEN_CRYPT 92
#define _SC_XOPEN_ENH_I18N 93
#define _SC_XOPEN_SHM 94
#define _SC_XOPEN_LEGACY 129
#define _SC_XOPEN_REALTIME 130
#define _SC_XOPEN_REALTIME_THREADS 131
#define _SC_XOPEN_STREAMS 246
#define _SC_CLOCK_SELECTION 137
#define _SC_MONOTONIC_CLOCK 149
#define _SC_THREAD_CPUTIME 139
#define _SC_THREAD_SPORADIC_SERVER 161
#define _SC_THREAD_ROBUST_PRIO_INHERIT 247
#define _SC_THREAD_ROBUST_PRIO_PROTECT 248
#define _SC_TIMEOUTS 164
#define _SC_SHELL 157
#define _SC_REALTIME_SIGNALS 9
#define _SC_PRIORITY_SCHEDULING 10
#define _SC_MEMLOCK 17
#define _SC_MEMLOCK_RANGE 18
#define _SC_MESSAGE_PASSING 20
#define _SC_SHARED_MEMORY_OBJECTS 22
#define _SC_ADVISORY_INFO 132
#define _SC_CPUTIME 138
#define _SC_READER_WRITER_LOCKS 153
#define _SC_SPIN_LOCKS 154
#define _SC_REGEXP 155
#define _SC_SPAWN 159
#define _SC_IPV6 235
#define _SC_RAW_SOCKETS 236
#define _SC_ASYNCHRONOUS_IO 12
#define _SC_2_C_BIND 47
#define _SC_2_C_DEV 48
#define _SC_2_FORT_DEV 49
#define _SC_2_FORT_RUN 50
#define _SC_2_CHAR_TERM 95
#define _SC_2_SW_DEV 51
#define _SC_2_LOCALEDEF 52
#define _SC_2_UPE 97
#define _SC_2_PBS 168
#define _SC_2_PBS_ACCOUNTING 169
#define _SC_2_PBS_LOCATE 170
#define _SC_2_PBS_MESSAGE 171
#define _SC_2_PBS_TRACK 172
#define _SC_2_PBS_CHECKPOINT 175
#define _SC_BARRIERS 133
#define _SC_SPORADIC_SERVER 160
#define _SC_TYPED_MEMORY_OBJECTS 165
#define _SC_TRACE 181
#define _SC_TRACE_EVENT_FILTER 182
#define _SC_TRACE_INHERIT 183
#define _SC_TRACE_LOG 184
#define _SC_V7_ILP32_OFF32 237
#define _SC_V7_ILP32_OFFBIG 238
#define _SC_V7_LP64_OFF64 239
#define _SC_V7_LPBIG_OFFBIG 240
#define _SC_SS_REPL_MAX 241
#define _SC_TRACE_EVENT_NAME_MAX 242
#define _SC_TRACE_NAME_MAX 243
#define _SC_TRACE_SYS_MAX 244
#define _SC_TRACE_USER_EVENT_MAX 245

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
#define _XOPEN_UNIX 1
#define _XOPEN_CRYPT -1
#define _XOPEN_ENH_I18N -1
#define _XOPEN_SHM 1
#define _XOPEN_LEGACY -1
#define _XOPEN_REALTIME -1
#define _XOPEN_REALTIME_THREADS -1
#define _XOPEN_STREAMS -1

// The options which are supported. Leaving one undefined means it has to be
// asked about at run time with sysconf; a value of -1 would mean it is not
// available at all.
#define _POSIX_CHOWN_RESTRICTED 1
#define _POSIX_THREADS _POSIX_VERSION
#define _POSIX_THREAD_SAFE_FUNCTIONS _POSIX_VERSION
#define _POSIX_THREAD_ATTR_STACKADDR _POSIX_VERSION
#define _POSIX_THREAD_ATTR_STACKSIZE _POSIX_VERSION
#define _POSIX_THREAD_PROCESS_SHARED _POSIX_VERSION
#define _POSIX_THREAD_PRIORITY_SCHEDULING _POSIX_VERSION
#define _POSIX_THREAD_CPUTIME 202405L
#define _POSIX_THREAD_PRIO_INHERIT _POSIX_VERSION
#define _POSIX_THREAD_PRIO_PROTECT -1
#define _POSIX_THREAD_ROBUST_PRIO_INHERIT -1
#define _POSIX_THREAD_ROBUST_PRIO_PROTECT -1
#define _POSIX_THREAD_SPORADIC_SERVER -1
#define _POSIX_JOB_CONTROL 1
#define _POSIX_SAVED_IDS 1
#define _POSIX_MAPPED_FILES 202405L
#define _POSIX_MEMORY_PROTECTION 202405L
#define _POSIX_FSYNC 202405L
#define _POSIX_SYNCHRONIZED_IO 202405L
#define _POSIX_TIMERS 202405L
#define _POSIX_MONOTONIC_CLOCK 202405L
#define _POSIX_CLOCK_SELECTION 202405L
#define _POSIX_SEMAPHORES 202405L
// sem_timedwait, pthread_mutex_timedlock, the rwlock pair and
// mq_timedsend are all here, which is the whole of this option.
#define _POSIX_TIMEOUTS 202405L
#define _POSIX_REALTIME_SIGNALS 202405L
#define _POSIX_PRIORITY_SCHEDULING 202405L
#define _POSIX_MEMLOCK 202405L
#define _POSIX_MEMLOCK_RANGE 202405L
#define _POSIX_MESSAGE_PASSING 202405L
#define _POSIX_SHARED_MEMORY_OBJECTS 202405L
#define _POSIX_ADVISORY_INFO 202405L
#define _POSIX_CPUTIME 202405L
#define _POSIX_READER_WRITER_LOCKS 202405L
#define _POSIX_SPIN_LOCKS 202405L
#define _POSIX_REGEXP 202405L
#define _POSIX_SPAWN 202405L
#define _POSIX_IPV6 202405L
#define _POSIX_RAW_SOCKETS 202405L
#define _POSIX_BARRIERS 202405L
#define _POSIX2_C_BIND 202405L
#define _POSIX2_C_DEV 202405L
#define _POSIX2_CHAR_TERM 202405L
// There is no asynchronous I/O, sporadic server, tracing or typed memory.
#define _POSIX_ASYNCHRONOUS_IO -1
#define _POSIX2_FORT_DEV -1
#define _POSIX2_FORT_RUN -1
#define _POSIX2_SW_DEV -1
#define _POSIX2_LOCALEDEF -1
#define _POSIX2_UPE -1
#define _POSIX2_PBS -1
#define _POSIX2_PBS_ACCOUNTING -1
#define _POSIX2_PBS_LOCATE -1
#define _POSIX2_PBS_MESSAGE -1
#define _POSIX2_PBS_TRACK -1
#define _POSIX2_PBS_CHECKPOINT -1
#define _POSIX_SPORADIC_SERVER -1
#define _POSIX_TRACE -1
#define _POSIX_TRACE_EVENT_FILTER -1
#define _POSIX_TRACE_INHERIT -1
#define _POSIX_TRACE_LOG -1
#define _POSIX_TYPED_MEMORY_OBJECTS -1
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
// syscall itself is not defined here. It is a name a program is entitled to
// give a member or a function of its own, and a function like macro would
// take it over wherever it appeared, so unistd.h declares the function that
// glibc and musl also declare. __syscall_helper is left for anything that
// wants to pass the arguments out in full.

#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression)                                         \
  (__extension__({                                                             \
    long __result;                                                             \
    do {                                                                       \
      __result = (long)(expression);                                           \
    } while (__result == -1L && errno == EINTR);                               \
    __result;                                                                  \
  }))
#endif // TEMP_FAILURE_RETRY

#define POSIX_CLOSE_RESTART 0

#endif // LLVM_LIBC_MACROS_LINUX_UNISTD_MACROS_H
