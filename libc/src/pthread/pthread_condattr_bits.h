//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Accessors for the packed word in pthread_condattr_t.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_PTHREAD_PTHREAD_CONDATTR_BITS_H
#define LLVM_LIBC_SRC_PTHREAD_PTHREAD_CONDATTR_BITS_H

#include "hdr/types/clockid_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <pthread.h> // pthread_condattr_t

namespace LIBC_NAMESPACE_DECL {
namespace condattr {

LIBC_INLINE int pshared(const pthread_condattr_t &a) {
  return static_cast<int>(a.__value & 1u);
}
LIBC_INLINE clockid_t clock(const pthread_condattr_t &a) {
  return static_cast<clockid_t>(a.__value >> 1);
}
LIBC_INLINE void set_pshared(pthread_condattr_t &a, int value) {
  a.__value = (a.__value & ~1u) | (static_cast<unsigned int>(value) & 1u);
}
LIBC_INLINE void set_clock(pthread_condattr_t &a, clockid_t clock) {
  a.__value = (a.__value & 1u) | (static_cast<unsigned int>(clock) << 1);
}

} // namespace condattr
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_PTHREAD_PTHREAD_CONDATTR_BITS_H
