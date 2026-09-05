//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct timex.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_TIMEX_H
#define LLVM_LIBC_TYPES_STRUCT_TIMEX_H

#include "struct_timeval.h"

// What adjtimex reads and writes. The modes field says which of the rest
// the call is to act on; everything else it reports as it stands. The
// trailing padding is the kernel's room to add fields later.
struct timex {
  unsigned int modes;
  long offset;    // Time offset, in microseconds or nanoseconds.
  long freq;      // Frequency offset, scaled.
  long maxerror;  // The largest error there could be.
  long esterror;  // The error there is thought to be.
  int status;     // What the clock is doing.
  long constant;  // The loop's time constant.
  long precision; // Read only from here down.
  long tolerance;
  struct timeval time; // Read only, except under ADJ_SETOFFSET.
  long tick;           // Microseconds between clock ticks.
  long ppsfreq;
  long jitter;
  int shift;
  long stabil;
  long jitcnt;
  long calcnt;
  long errcnt;
  long stbcnt;

  int tai; // The offset from TAI.

  int : 32;
  int : 32;
  int : 32;
  int : 32;
  int : 32;
  int : 32;
  int : 32;
  int : 32;
  int : 32;
  int : 32;
  int : 32;
};

#endif // LLVM_LIBC_TYPES_STRUCT_TIMEX_H
