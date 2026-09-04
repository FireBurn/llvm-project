//===-- Definition of macros from sys/time.h ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_SYS_TIME_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_SYS_TIME_MACROS_H

// Timer types for setitimer(2).
#define ITIMER_REAL 0
#define ITIMER_VIRTUAL 1
#define ITIMER_PROF 2

// Add two timevals and put the result in timeval_ptr_result. If the resulting
// usec value is greater than 999,999 then the microseconds are turned into full
// seconds (1,000,000 is subtracted from usec and 1 is added to sec).
//
// This and timersub are more than one statement, so each is wrapped to make
// it one, or a caller writing "if (c) timeradd(...); else" would not compile.
#define timeradd(timeval_ptr_a, timeval_ptr_b, timeval_ptr_result)             \
  do {                                                                         \
    (timeval_ptr_result)->tv_sec =                                             \
        (timeval_ptr_a)->tv_sec + (timeval_ptr_b)->tv_sec;                     \
    (timeval_ptr_result)->tv_usec =                                            \
        (timeval_ptr_a)->tv_usec + (timeval_ptr_b)->tv_usec;                   \
    if ((timeval_ptr_result)->tv_usec >= 1000000) {                            \
      ++(timeval_ptr_result)->tv_sec;                                          \
      (timeval_ptr_result)->tv_usec -= 1000000;                                \
    }                                                                          \
  } while (0)

// Subtract timeval_ptr_b from timeval_ptr_a and put the result in
// timeval_ptr_result, borrowing a second when the microseconds go negative.
#define timersub(timeval_ptr_a, timeval_ptr_b, timeval_ptr_result)             \
  do {                                                                         \
    (timeval_ptr_result)->tv_sec =                                             \
        (timeval_ptr_a)->tv_sec - (timeval_ptr_b)->tv_sec;                     \
    (timeval_ptr_result)->tv_usec =                                            \
        (timeval_ptr_a)->tv_usec - (timeval_ptr_b)->tv_usec;                   \
    if ((timeval_ptr_result)->tv_usec < 0) {                                   \
      --(timeval_ptr_result)->tv_sec;                                          \
      (timeval_ptr_result)->tv_usec += 1000000;                                \
    }                                                                          \
  } while (0)

// Reset a timeval to the epoch.
#define timerclear(timeval_ptr)                                                \
  ((timeval_ptr)->tv_sec = (timeval_ptr)->tv_usec = 0)

// Determine if a timeval is set to something other than the epoch. This is
// an expression, since every use of it is a condition.
#define timerisset(timeval_ptr)                                                \
  ((timeval_ptr)->tv_sec != 0 || (timeval_ptr)->tv_usec != 0)

// Compare two timevals using CMP.
#define timercmp(timeval_ptr_a, timeval_ptr_b, CMP)                            \
  (((timeval_ptr_a)->tv_sec == (timeval_ptr_b)->tv_sec)                        \
       ? ((timeval_ptr_a)->tv_usec CMP(timeval_ptr_b)->tv_usec)                \
       : ((timeval_ptr_a)->tv_sec CMP(timeval_ptr_b)->tv_sec))

// Convert between the two ways of writing a time, which differ in whether
// the fraction is microseconds or nanoseconds.
#define TIMEVAL_TO_TIMESPEC(tv, ts)                                            \
  do {                                                                         \
    (ts)->tv_sec = (tv)->tv_sec;                                               \
    (ts)->tv_nsec = (tv)->tv_usec * 1000;                                      \
  } while (0)

#define TIMESPEC_TO_TIMEVAL(tv, ts)                                            \
  do {                                                                         \
    (tv)->tv_sec = (ts)->tv_sec;                                               \
    (tv)->tv_usec = (ts)->tv_nsec / 1000;                                      \
  } while (0)

#endif // LLVM_LIBC_MACROS_LINUX_SYS_TIME_MACROS_H
