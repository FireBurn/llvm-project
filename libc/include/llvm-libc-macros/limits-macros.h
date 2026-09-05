//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of macros from limits.h.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LIMITS_MACROS_H
#define LLVM_LIBC_MACROS_LIMITS_MACROS_H

// Define all C23 macro constants of limits.h

#ifndef CHAR_BIT
#ifdef __CHAR_BIT__
#define CHAR_BIT __CHAR_BIT__
#else
#define CHAR_BIT 8
#endif // __CHAR_BIT__
#endif // CHAR_BIT

#ifndef MB_LEN_MAX
// Represents a single UTF-32 wide character in the default locale.
#define MB_LEN_MAX 4
#endif // MB_LEN_MAX

// *_WIDTH macros

#ifndef CHAR_WIDTH
#define CHAR_WIDTH CHAR_BIT
#endif // CHAR_WIDTH

#ifndef SCHAR_WIDTH
#define SCHAR_WIDTH CHAR_BIT
#endif // SCHAR_WIDTH

#ifndef UCHAR_WIDTH
#define UCHAR_WIDTH CHAR_BIT
#endif // UCHAR_WIDTH

#ifndef SHRT_WIDTH
#ifdef __SHRT_WIDTH__
#define SHRT_WIDTH __SHRT_WIDTH__
#else
#define SHRT_WIDTH 16
#endif // __SHRT_WIDTH__
#endif // SHRT_WIDTH

#ifndef USHRT_WIDTH
#define USHRT_WIDTH SHRT_WIDTH
#endif // USHRT_WIDTH

#ifndef INT_WIDTH
#ifdef __INT_WIDTH__
#define INT_WIDTH __INT_WIDTH__
#else
#define INT_WIDTH 32
#endif // __INT_WIDTH__
#endif // INT_WIDTH

#ifndef UINT_WIDTH
#define UINT_WIDTH INT_WIDTH
#endif // UINT_WIDTH

#ifndef LONG_WIDTH
#ifdef __LONG_WIDTH__
#define LONG_WIDTH __LONG_WIDTH__
#elif defined(__WORDSIZE)
#define LONG_WIDTH __WORDSIZE
#else
// Use __SIZEOF_LONG__ * CHAR_BIT as backup.  This is needed for clang-13 or
// before.
#define LONG_WIDTH (__SIZEOF_LONG__ * CHAR_BIT)
#endif // __LONG_WIDTH__
#endif // LONG_WIDTH

#ifndef ULONG_WIDTH
#define ULONG_WIDTH LONG_WIDTH
#endif // ULONG_WIDTH

#ifndef LLONG_WIDTH
#ifdef __LLONG_WIDTH__
#define LLONG_WIDTH __LLONG_WIDTH__
#else
#define LLONG_WIDTH 64
#endif // __LLONG_WIDTH__
#endif // LLONG_WIDTH

#ifndef ULLONG_WIDTH
#define ULLONG_WIDTH LLONG_WIDTH
#endif // ULLONG_WIDTH

#ifndef BOOL_WIDTH
#ifdef __BOOL_WIDTH__
#define BOOL_WIDTH __BOOL_WIDTH__
#else
#define BOOL_WIDTH 1
#endif // __BOOL_WIDTH__
#endif // BOOL_WIDTH

// *_MAX macros

#ifndef SCHAR_MAX
#ifdef __SCHAR_MAX__
#define SCHAR_MAX __SCHAR_MAX__
#else
#define SCHAR_MAX 0x7f
#endif // __SCHAR_MAX__
#endif // SCHAR_MAX

#ifndef UCHAR_MAX
#define UCHAR_MAX (SCHAR_MAX * 2 + 1)
#endif // UCHAR_MAX

// Check if char is unsigned.
#if !defined(__CHAR_UNSIGNED__) && ('\xff' > 0)
#define __CHAR_UNSIGNED__
#endif

#ifndef CHAR_MAX
#ifdef __CHAR_UNSIGNED__
#define CHAR_MAX UCHAR_MAX
#else
#define CHAR_MAX SCHAR_MAX
#endif // __CHAR_UNSIGNED__
#endif // CHAR_MAX

#ifndef SHRT_MAX
#ifdef __SHRT_MAX__
#define SHRT_MAX __SHRT_MAX__
#else
#define SHRT_MAX 0x7fff
#endif // __SHRT_MAX__
#endif // SHRT_MAX

#ifndef USHRT_MAX
#define USHRT_MAX (SHRT_MAX * 2U + 1U)
#endif // USHRT_MAX

#ifndef INT_MAX
#ifdef __INT_MAX__
#define INT_MAX __INT_MAX__
#else
#define INT_MAX (0 ^ (1 << (INT_WIDTH - 1)))
#endif // __INT_MAX__
#endif // INT_MAX

#ifndef UINT_MAX
#define UINT_MAX (INT_MAX * 2U + 1U)
#endif // UINT_MAX

#ifndef LONG_MAX
#ifdef __LONG_MAX__
#define LONG_MAX __LONG_MAX__
#else
#define LONG_MAX (0L ^ (1L << (LONG_WIDTH - 1)))
#endif // __LONG_MAX__
#endif // LONG_MAX

#ifndef ULONG_MAX
#define ULONG_MAX (LONG_MAX * 2UL + 1UL)
#endif // ULONG_MAX

#ifndef LLONG_MAX
#ifdef __LONG_LONG_MAX__
#define LLONG_MAX __LONG_LONG_MAX__
#else
#define LLONG_MAX (0LL ^ (1LL << (LLONG_WIDTH - 1)))
#endif // __LONG_LONG_MAX__
#endif // LLONG_MAX

#ifndef ULLONG_MAX
#define ULLONG_MAX (LLONG_MAX * 2ULL + 1ULL)
#endif // ULLONG_MAX

// *_MIN macros

#ifndef SCHAR_MIN
#define SCHAR_MIN (-SCHAR_MAX - 1)
#endif // SCHAR_MIN

#ifndef UCHAR_MIN
#define UCHAR_MIN 0
#endif // UCHAR_MIN

#ifndef CHAR_MIN
#ifdef __CHAR_UNSIGNED__
#define CHAR_MIN UCHAR_MIN
#else
#define CHAR_MIN SCHAR_MIN
#endif // __CHAR_UNSIGNED__
#endif // CHAR_MIN

#ifndef SHRT_MIN
#define SHRT_MIN (-SHRT_MAX - 1)
#endif // SHRT_MIN

#ifndef USHRT_MIN
#define USHRT_MIN 0U
#endif // USHRT_MIN

#ifndef INT_MIN
#define INT_MIN (-INT_MAX - 1)
#endif // INT_MIN

#ifndef UINT_MIN
#define UINT_MIN 0U
#endif // UINT_MIN

#ifndef LONG_MIN
#define LONG_MIN (-LONG_MAX - 1L)
#endif // LONG_MIN

#ifndef ULONG_MIN
#define ULONG_MIN 0UL
#endif // ULONG_MIN

#ifndef LLONG_MIN
#define LLONG_MIN (-LLONG_MAX - 1LL)
#endif // LLONG_MIN

#ifndef ULLONG_MIN
#define ULLONG_MIN 0ULL
#endif // ULLONG_MIN

#ifndef _POSIX_MAX_CANON
#define _POSIX_MAX_CANON 255
#endif

#ifndef _POSIX_MAX_INPUT
#define _POSIX_MAX_INPUT 255
#endif

#ifndef _POSIX_NAME_MAX
#define _POSIX_NAME_MAX 14
#endif

#ifndef _POSIX_PATH_MAX
#define _POSIX_PATH_MAX 256
#endif

#ifndef _POSIX_NGROUPS_MAX
#define _POSIX_NGROUPS_MAX 8
#endif

#ifndef _POSIX_THREAD_DESTRUCTOR_ITERATIONS
#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS 4
#endif

#ifndef PTHREAD_DESTRUCTOR_ITERATIONS
#define PTHREAD_DESTRUCTOR_ITERATIONS _POSIX_THREAD_DESTRUCTOR_ITERATIONS
#endif

#ifndef _POSIX_HOST_NAME_MAX
#define _POSIX_HOST_NAME_MAX 255
#endif

#ifndef SEM_VALUE_MAX
/// The maximum value a semaphore may hold.
#define SEM_VALUE_MAX INT_MAX
#endif

#ifdef __linux__

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif // PATH_MAX

#ifndef NAME_MAX
#define NAME_MAX 255
#endif // NAME_MAX

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 64
#endif // HOST_NAME_MAX

/// The number of supplementary group IDs a process may have. Linux caps this
/// at the value of /proc/sys/kernel/ngroups_max, which cannot exceed this.
#ifndef NGROUPS_MAX
#define NGROUPS_MAX 65536
#endif // NGROUPS_MAX

#ifndef MAX_CANON
#define MAX_CANON 255
#endif // MAX_CANON

#ifndef MAX_INPUT
#define MAX_INPUT 255
#endif // MAX_INPUT

/// The most a write to a pipe can transfer without being interleaved with
/// another writer's.
#ifndef PIPE_BUF
#define PIPE_BUF 4096
#endif // PIPE_BUF

#ifndef LOGIN_NAME_MAX
#define LOGIN_NAME_MAX 256
#endif // LOGIN_NAME_MAX

#ifndef TTY_NAME_MAX
#define TTY_NAME_MAX 32
#endif // TTY_NAME_MAX

#ifndef MQ_PRIO_MAX
#define MQ_PRIO_MAX 32768
#endif // MQ_PRIO_MAX

#ifndef RTSIG_MAX
#define RTSIG_MAX 32
#endif // RTSIG_MAX

#ifndef DELAYTIMER_MAX
#define DELAYTIMER_MAX 2147483647
#endif // DELAYTIMER_MAX

#ifndef AIO_PRIO_DELTA_MAX
#define AIO_PRIO_DELTA_MAX 20
#endif // AIO_PRIO_DELTA_MAX

#ifndef PTHREAD_KEYS_MAX
#define PTHREAD_KEYS_MAX 1024
#endif // PTHREAD_KEYS_MAX

#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN 16384
#endif // PTHREAD_STACK_MIN

/// The extended attribute limits the kernel enforces.
#ifndef XATTR_NAME_MAX
#define XATTR_NAME_MAX 255
#endif // XATTR_NAME_MAX

#ifndef XATTR_SIZE_MAX
#define XATTR_SIZE_MAX 65536
#endif // XATTR_SIZE_MAX

#ifndef XATTR_LIST_MAX
#define XATTR_LIST_MAX 65536
#endif // XATTR_LIST_MAX

#ifndef CHARCLASS_NAME_MAX
#define CHARCLASS_NAME_MAX 2048
#endif // CHARCLASS_NAME_MAX

/// The largest repetition count a regular expression bound may ask for. This
/// is what the regex implementation here accepts, which is the minimum POSIX
/// requires rather than the larger value some other implementations offer.
#ifndef RE_DUP_MAX
#define RE_DUP_MAX 255
#endif // RE_DUP_MAX

#endif // __linux__

#ifndef _POSIX_ARG_MAX
#define _POSIX_ARG_MAX 4096
#endif

#ifndef _POSIX_ATEXIT_MAX
#define _POSIX_ATEXIT_MAX 32
#endif

/// The POSIX minimum for the utilities, which is a separate set from the
/// values above.
// The length of a line a text utility has to be able to handle. POSIX gives
// a floor of 2048 and lets an implementation raise it; Linux does not.
#ifndef LINE_MAX
#define LINE_MAX 2048
#endif

#ifndef _POSIX2_LINE_MAX
#define _POSIX2_LINE_MAX 2048
#endif

/// The POSIX minimum values. An implementation must support at least these,
/// so portable code sizes its buffers from them rather than from the values
/// this implementation happens to offer.
#ifndef _POSIX_AIO_LISTIO_MAX
#define _POSIX_AIO_LISTIO_MAX 2
#endif

#ifndef _POSIX_AIO_MAX
#define _POSIX_AIO_MAX 1
#endif

#ifndef _POSIX_CHILD_MAX
#define _POSIX_CHILD_MAX 25
#endif

#ifndef _POSIX_CLOCKRES_MIN
#define _POSIX_CLOCKRES_MIN 20000000
#endif

#ifndef _POSIX_DELAYTIMER_MAX
#define _POSIX_DELAYTIMER_MAX 32
#endif

#ifndef _POSIX_LINK_MAX
#define _POSIX_LINK_MAX 8
#endif

#ifndef _POSIX_LOGIN_NAME_MAX
#define _POSIX_LOGIN_NAME_MAX 9
#endif

#ifndef _POSIX_MAX_CANON
#define _POSIX_MAX_CANON 255
#endif

#ifndef _POSIX_MQ_OPEN_MAX
#define _POSIX_MQ_OPEN_MAX 8
#endif

#ifndef _POSIX_MQ_PRIO_MAX
#define _POSIX_MQ_PRIO_MAX 32
#endif

#ifndef _POSIX_OPEN_MAX
#define _POSIX_OPEN_MAX 20
#endif

#ifndef _POSIX_PIPE_BUF
#define _POSIX_PIPE_BUF 512
#endif

#ifndef _POSIX_RE_DUP_MAX
#define _POSIX_RE_DUP_MAX 255
#endif

#ifndef _POSIX_RTSIG_MAX
#define _POSIX_RTSIG_MAX 8
#endif

#ifndef _POSIX_SEM_NSEMS_MAX
#define _POSIX_SEM_NSEMS_MAX 256
#endif

#ifndef _POSIX_SEM_VALUE_MAX
#define _POSIX_SEM_VALUE_MAX 32767
#endif

#ifndef _POSIX_SIGQUEUE_MAX
#define _POSIX_SIGQUEUE_MAX 32
#endif

#ifndef _POSIX_SSIZE_MAX
#define _POSIX_SSIZE_MAX 32767
#endif

#ifndef _POSIX_STREAM_MAX
#define _POSIX_STREAM_MAX 8
#endif

#ifndef _POSIX_SYMLINK_MAX
#define _POSIX_SYMLINK_MAX 255
#endif

#ifndef _POSIX_SYMLOOP_MAX
#define _POSIX_SYMLOOP_MAX 8
#endif

#ifndef _POSIX_THREAD_KEYS_MAX
#define _POSIX_THREAD_KEYS_MAX 128
#endif

#ifndef _POSIX_THREAD_THREADS_MAX
#define _POSIX_THREAD_THREADS_MAX 64
#endif

#ifndef _POSIX_TIMER_MAX
#define _POSIX_TIMER_MAX 32
#endif

#ifndef _POSIX_TTY_NAME_MAX
#define _POSIX_TTY_NAME_MAX 9
#endif

#ifndef _POSIX_TZNAME_MAX
#define _POSIX_TZNAME_MAX 6
#endif

#ifndef IOV_MAX
#define IOV_MAX 1024
#endif // IOV_MAX

#ifndef SSIZE_MAX
#ifdef __PTRDIFF_MAX__
/// The maximum value that can be stored in an object of type ssize_t.
#define SSIZE_MAX __PTRDIFF_MAX__
#endif
#endif

#endif // LLVM_LIBC_MACROS_LIMITS_MACROS_H
