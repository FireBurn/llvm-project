//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The message that abort reports, as set by android_set_abort_message.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_ABORT_MESSAGE_H
#define LLVM_LIBC_SRC___SUPPORT_ABORT_MESSAGE_H

#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <stddef.h>

namespace LIBC_NAMESPACE_DECL {
namespace abort_message {

LIBC_INLINE constexpr size_t CAPACITY = 4096;

// The buffer is process wide and unsynchronised: it is written just before a
// deliberate abort, and a racing writer can at worst garble its own message.
LIBC_INLINE char *buffer() {
  static char storage[CAPACITY];
  return storage;
}

// Copies the message, truncating it to fit.
LIBC_INLINE void set(const char *msg) {
  char *dst = buffer();
  size_t i = 0;
  for (; msg[i] != '\0' && i < CAPACITY - 1; ++i)
    dst[i] = msg[i];
  dst[i] = '\0';
}

// Returns the message, or null when none was set.
LIBC_INLINE const char *get() {
  return buffer()[0] == '\0' ? nullptr : buffer();
}

} // namespace abort_message
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_ABORT_MESSAGE_H
