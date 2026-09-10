//===-- The generator behind the 48 bit random family -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_STDLIB_RAND48_H
#define LLVM_LIBC_SRC_STDLIB_RAND48_H

#include "hdr/stdint_proxy.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

// X(n+1) = (a * X(n) + c) mod 2^48, with the multiplier and the addend the
// standard names. lcong48 is allowed to replace both, so they are state as
// much as the sequence itself is.
constexpr uint64_t RAND48_MULTIPLIER = 0x5deece66dULL;
constexpr uint64_t RAND48_ADDEND = 0xbULL;
constexpr uint64_t RAND48_MASK = 0xffffffffffffULL;

// The seed the standard says srand48 leaves in the low sixteen bits.
constexpr uint16_t RAND48_SEED_LOW = 0x330e;

struct Rand48State {
  uint64_t multiplier = RAND48_MULTIPLIER;
  uint64_t addend = RAND48_ADDEND;
  uint16_t xsubi[3] = {RAND48_SEED_LOW, 0, 0};
};

extern Rand48State rand48_state;

// Reads three sixteen bit words as one forty eight bit number, and puts one
// back. The words are least significant first, which is the order the
// interface hands them over in.
LIBC_INLINE uint64_t rand48_pack(const unsigned short xsubi[3]) {
  return (static_cast<uint64_t>(xsubi[2]) << 32) |
         (static_cast<uint64_t>(xsubi[1]) << 16) |
         static_cast<uint64_t>(xsubi[0]);
}

LIBC_INLINE void rand48_unpack(unsigned short xsubi[3], uint64_t value) {
  xsubi[0] = static_cast<unsigned short>(value & 0xffff);
  xsubi[1] = static_cast<unsigned short>((value >> 16) & 0xffff);
  xsubi[2] = static_cast<unsigned short>((value >> 32) & 0xffff);
}

// One step of the sequence, leaving the new value where it was read from and
// handing it back.
LIBC_INLINE uint64_t rand48_step(unsigned short xsubi[3]) {
  uint64_t next =
      (rand48_state.multiplier * rand48_pack(xsubi) + rand48_state.addend) &
      RAND48_MASK;
  rand48_unpack(xsubi, next);
  return next;
}

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_STDLIB_RAND48_H
