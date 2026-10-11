//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Trigonometric functions on the x87 FPU
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_FPUTIL_X86_64_X87_TRIG_H
#define LLVM_LIBC_SRC___SUPPORT_FPUTIL_X86_64_X87_TRIG_H

#include "src/__support/CPP/bit.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/x86_64/x87_two_over_pi.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/properties/architectures.h"
#include "src/__support/macros/properties/types.h"

#if !defined(LIBC_TARGET_ARCH_IS_X86_64) ||                                    \
    !defined(LIBC_TYPES_LONG_DOUBLE_IS_X86_FLOAT80)
#error "Invalid include"
#endif

namespace LIBC_NAMESPACE_DECL {
namespace fputil {
namespace x87 {

// fsin, fcos and fptan are accurate on [-pi/4, pi/4] only: beyond it their
// internal 66-bit pi loses a bit of precision per octave. Larger arguments are
// reduced here first, so the instructions only ever see the small range.

namespace internal {

// Bits [lowbit, lowbit + 64) of the 320-bit integer in `limbs`.
LIBC_INLINE uint64_t get64(const uint64_t (&limbs)[5], unsigned lowbit) {
  unsigned limb = lowbit / 64, off = lowbit % 64;
  uint64_t v = limb < 5 ? limbs[limb] >> off : 0;
  if (off != 0 && limb + 1 < 5)
    v |= limbs[limb + 1] << (64 - off);
  return v;
}

// Payne-Hanek reduction of x >= pi/4. Returns r in [-pi/4, pi/4] with
// x = (n + 4k) * pi/2 + r, and n in [0, 4).
LIBC_INLINE long double reduce(long double x, unsigned &n) {
  using FPBits = FPBits<long double>;
  FPBits bits(x);
  uint64_t mant = static_cast<uint64_t>(bits.get_explicit_mantissa());
  int e = bits.get_biased_exponent() - FPBits::EXP_BIAS - 63; // x = mant * 2^e

  // Terms of 2 / pi above 2^1 are multiples of 4 and drop out. The 256 bits
  // below that, times the 64-bit mantissa, leave more than 128 fraction bits.
  unsigned first = e > 2 ? e - 1 : 1;
  unsigned point = first + 255 - e; // x * 2 / pi = product * 2^-point

  unsigned off = first - 1;
  unsigned w = off / 64, sh = off % 64;
  uint64_t win[4];
  for (unsigned j = 0; j < 4; ++j)
    win[j] = sh == 0 ? TWO_OVER_PI[w + j]
                     : (TWO_OVER_PI[w + j] << sh) |
                           (TWO_OVER_PI[w + j + 1] >> (64 - sh));

  uint64_t product[5] = {0, 0, 0, 0, 0};
  for (unsigned j = 0; j < 4; ++j) {
    __uint128_t t = static_cast<__uint128_t>(mant) * win[3 - j];
    uint64_t lo = static_cast<uint64_t>(t), hi = static_cast<uint64_t>(t >> 64);
    unsigned pos = j;
    uint64_t carry = 0;
    product[pos] += lo;
    carry = product[pos] < lo;
    ++pos;
    uint64_t add = hi + carry; // cannot overflow: hi < 2^64 - 1
    product[pos] += add;
    carry = product[pos] < add;
    while (carry && ++pos < 5) {
      ++product[pos];
      carry = product[pos] == 0;
    }
  }

  n = static_cast<unsigned>(get64(product, point) & 3);
  uint64_t fhi = get64(product, point - 64);
  uint64_t flo = get64(product, point - 128);

  // Round to the nearest multiple of pi/2.
  bool negative = false;
  if (fhi >> 63) {
    n = (n + 1) & 3;
    flo = ~flo + 1;
    fhi = ~fhi + (flo == 0);
    negative = true;
  }
  if (fhi == 0 && flo == 0)
    return 0.0L;

  int lz = fhi != 0 ? cpp::countl_zero(fhi) : 64 + cpp::countl_zero(flo);
  if (lz >= 64) {
    fhi = flo << (lz - 64);
    flo = 0;
  } else if (lz > 0) {
    fhi = (fhi << lz) | (flo >> (64 - lz));
    flo <<= lz;
  }

  // f = (fhi + flo * 2^-64) * 2^-(64 + lz); r = f * pi/2 with pi/2 held as
  // 0xc90fdaa22168c234 * 2^-63 + 0xc4c6628b80dc1cd1 * 2^-127.
  long double a = static_cast<long double>(fhi);
  long double b = static_cast<long double>(flo);
  constexpr long double PIO2_HI = 0xc90fdaa22168c234ULL;
  constexpr long double PIO2_LO = 0xc4c6628b80dc1cd1ULL;
  long double r = a * PIO2_HI + (a * PIO2_LO + b * PIO2_HI) * 0x1.0p-64L;
  FPBits scale = FPBits::one();
  scale.set_biased_exponent(FPBits::EXP_BIAS - 127 - lz);
  r *= scale.get_val();
  return negative ? -r : r;
}

LIBC_INLINE long double fsin(long double x) {
  long double r;
  asm("fsin" : "=t"(r) : "0"(x));
  return r;
}

LIBC_INLINE long double fcos(long double x) {
  long double r;
  asm("fcos" : "=t"(r) : "0"(x));
  return r;
}

LIBC_INLINE long double ftan(long double x) {
  long double r;
  asm("fptan\n\t"
      "fstp %%st(0)"
      : "=t"(r)
      : "0"(x));
  return r;
}

} // namespace internal

// Handles NaN and infinity. Returns true when `result` holds the answer.
LIBC_INLINE bool special(long double x, long double &result) {
  using FPBits = FPBits<long double>;
  FPBits bits(x);
  if (LIBC_LIKELY(!bits.is_inf_or_nan()))
    return false;
  if (bits.is_signaling_nan()) {
    raise_except_if_required(FE_INVALID);
    result = FPBits::quiet_nan().get_val();
    return true;
  }
  if (bits.is_inf()) {
    set_errno_if_required(EDOM);
    raise_except_if_required(FE_INVALID);
  }
  result = x + FPBits::quiet_nan().get_val();
  return true;
}

// pi/4, rounded down.
constexpr long double PI_OVER_4 = 0x1.921fb54442d18468p-1L;

LIBC_INLINE long double sin(long double x) {
  if (FPBits<long double>(x).abs().get_val() <= PI_OVER_4)
    return internal::fsin(x);
  unsigned n;
  long double r = internal::reduce(FPBits<long double>(x).abs().get_val(), n);
  long double s;
  switch (n) {
  case 0:
    s = internal::fsin(r);
    break;
  case 1:
    s = internal::fcos(r);
    break;
  case 2:
    s = -internal::fsin(r);
    break;
  default:
    s = -internal::fcos(r);
    break;
  }
  return FPBits<long double>(x).is_neg() ? -s : s;
}

LIBC_INLINE long double cos(long double x) {
  if (FPBits<long double>(x).abs().get_val() <= PI_OVER_4)
    return internal::fcos(x);
  unsigned n;
  long double r = internal::reduce(FPBits<long double>(x).abs().get_val(), n);
  switch (n) {
  case 0:
    return internal::fcos(r);
  case 1:
    return -internal::fsin(r);
  case 2:
    return -internal::fcos(r);
  default:
    return internal::fsin(r);
  }
}

LIBC_INLINE long double tan(long double x) {
  if (FPBits<long double>(x).abs().get_val() <= PI_OVER_4)
    return internal::ftan(x);
  unsigned n;
  long double r = internal::reduce(FPBits<long double>(x).abs().get_val(), n);
  long double t = internal::ftan(r);
  if (n & 1)
    t = -1.0L / t;
  return FPBits<long double>(x).is_neg() ? -t : t;
}

} // namespace x87
} // namespace fputil
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_FPUTIL_X86_64_X87_TRIG_H
