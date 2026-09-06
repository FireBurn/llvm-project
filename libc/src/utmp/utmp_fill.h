//===-- Filling in a login record -------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_UTMP_UTMP_FILL_H
#define LLVM_LIBC_SRC_UTMP_UTMP_FILL_H

#include "hdr/time_macros.h"
#include "hdr/types/struct_utmp.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/time/clock_gettime.h"

namespace LIBC_NAMESPACE_DECL {
namespace utmp_fill {

// Copies at most `size` bytes and pads the rest with zeroes, which is how a
// record's character fields are written: a name which fills the array is not
// terminated, and a shorter one leaves no stale bytes behind it.
LIBC_INLINE void field(char *dest, size_t size, const char *src) {
  size_t i = 0;
  if (src != nullptr)
    for (; i < size && src[i] != '\0'; ++i)
      dest[i] = src[i];
  for (; i < size; ++i)
    dest[i] = '\0';
}

// The record's time, which is two 32 bit fields rather than a timeval so
// that the file reads the same on a 32 and a 64 bit machine.
LIBC_INLINE void stamp(struct utmp *record) {
  timespec now = {};
  if (!internal::clock_gettime(CLOCK_REALTIME, &now).has_value())
    now = {};
  record->ut_tv.tv_sec = static_cast<int32_t>(now.tv_sec);
  record->ut_tv.tv_usec = static_cast<int32_t>(now.tv_nsec / 1000);
}

} // namespace utmp_fill
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_UTMP_UTMP_FILL_H
