//===-- Implementation header for rand utilities ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_STDLIB_RAND_UTIL_H
#define LLVM_LIBC_SRC_STDLIB_RAND_UTIL_H

#include "hdr/stdint_proxy.h"
#include "src/__support/CPP/atomic.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The ISO C standard does not explicitly require thread-safe behavior for the
// generic `rand()` function. Some implementations expect it however, so we
// provide it here.
extern cpp::Atomic<unsigned long> rand_next;

// Turns a seed into a generator state.
//
// The generator is a xorshift, which has no way out of a state of all zero
// bits: it stays there and returns nothing but zero from then on. A seed is
// allowed to be zero, and zero is the one people reach for when they want a
// run they can repeat, so the seed is mixed rather than used as the state
// directly. The mixer is the one the generator's author recommends for
// seeding it, and being a bijection it gives every seed a state of its own.
LIBC_INLINE constexpr unsigned long rand_state_for(unsigned int seed) {
  if constexpr (sizeof(void *) == sizeof(uint64_t)) {
    uint64_t z = static_cast<uint64_t>(seed) + 0x9E3779B97F4A7C15ull;
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    z = z ^ (z >> 31);
    return static_cast<unsigned long>(z);
  } else {
    uint32_t z = static_cast<uint32_t>(seed) + 0x9E3779B9u;
    z = (z ^ (z >> 16)) * 0x85EBCA6Bu;
    z = (z ^ (z >> 13)) * 0xC2B2AE35u;
    z = z ^ (z >> 16);
    return static_cast<unsigned long>(z);
  }
}

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_STDLIB_RAND_UTIL_H
