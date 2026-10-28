//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Formatting for strfmon and strfmon_l
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_MONETARY_STRFMON_IMPL_H
#define LLVM_LIBC_SRC_MONETARY_STRFMON_IMPL_H

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "hdr/types/ssize_t.h"
#include "src/__support/CPP/limits.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/locale/localeconv.h"
#include "src/stdio/vsnprintf.h"

#include <locale.h>
#include <stdarg.h>

namespace LIBC_NAMESPACE_DECL {
namespace monetary {

LIBC_INLINE int format_fixed(char *buffer, size_t size, const char *format,
                             ...) {
  va_list vlist;
  va_start(vlist, format);
  int ret = internal::vsnprintf_impl(buffer, size, format, vlist);
  va_end(vlist);
  return ret;
}

// The characters of the output, which stops taking them when the buffer is
// full but goes on counting, so that the caller can tell.
class Output {
  char *buf;
  size_t size;
  size_t used = 0;

public:
  Output(char *buf, size_t size) : buf(buf), size(size) {}
  void put(char c) {
    // One byte is kept for the terminator.
    if (used + 1 < size)
      buf[used] = c;
    ++used;
  }
  void put(const char *s) {
    for (; *s != '\0'; ++s)
      put(*s);
  }
  void put_n(char c, size_t n) {
    for (size_t i = 0; i < n; ++i)
      put(c);
  }
  size_t count() const { return used; }
  bool fits() const { return size != 0 && used < size; }
  void finish() {
    if (size != 0)
      buf[used < size ? used : size - 1] = '\0';
  }
};

LIBC_INLINE size_t length(const char *s) {
  size_t n = 0;
  while (s[n] != '\0')
    ++n;
  return n;
}

// What one conversion asks for.
struct Spec {
  char fill = ' ';
  bool group = true;
  bool parentheses = false;
  bool symbol = true;
  bool left_justify = false;
  int width = 0;
  int left_precision = -1;
  int right_precision = -1;
  bool international = false;
};

LIBC_INLINE bool parse_number(const char *&p, int &value) {
  value = 0;
  bool any = false;
  while (*p >= '0' && *p <= '9') {
    if (value > 100000)
      return false;
    value = value * 10 + (*p - '0');
    ++p;
    any = true;
  }
  return any;
}

// Formats one amount into `out`, which is as wide as the number alone.
// Returns false when the number does not fit in what is available for it.
LIBC_INLINE bool format_amount(const Spec &spec, double value,
                               const struct lconv &lc, Output &out) {
  const bool negative = value < 0;
  const double magnitude = negative ? -value : value;

  int digits = spec.right_precision;
  if (digits < 0) {
    signed char frac = spec.international ? lc.int_frac_digits : lc.frac_digits;
    digits = frac == cpp::numeric_limits<char>::max() || frac < 0 ? 2 : frac;
  }

  // The magnitude as digits, with the integer part first.
  char number[512];
  int n = format_fixed(number, sizeof(number), "%.*f", digits, magnitude);
  if (n < 0 || static_cast<size_t>(n) >= sizeof(number))
    return false;
  size_t int_len = 0;
  while (number[int_len] >= '0' && number[int_len] <= '9')
    ++int_len;

  const char *point =
      lc.mon_decimal_point[0] != '\0' ? lc.mon_decimal_point : lc.decimal_point;
  const char *sep = lc.mon_thousands_sep;
  const char *grouping = lc.mon_grouping;

  // How the integer part is split, from the last digit towards the first.
  // The grouping string gives each size in turn; the last repeats, and a
  // size of CHAR_MAX stops the grouping.
  char integer[1024];
  size_t pos = 0;
  size_t pad = 0;
  if (spec.left_precision >= 0 &&
      static_cast<size_t>(spec.left_precision) > int_len)
    pad = static_cast<size_t>(spec.left_precision) - int_len;
  const size_t total_digits = int_len + pad;
  bool grouped = spec.group && sep[0] != '\0' && grouping[0] != '\0';
  size_t group_size = grouped ? static_cast<unsigned char>(grouping[0]) : 0;
  const char *next_group = grouped ? grouping + 1 : grouping;
  if (group_size == 0 || group_size == static_cast<unsigned char>(
                                           cpp::numeric_limits<char>::max()))
    grouped = false;
  size_t sep_len = length(sep);
  size_t in_group = 0;
  // Build it backwards into integer[] and reverse after.
  for (size_t i = 0; i < total_digits; ++i) {
    if (grouped && in_group == group_size) {
      for (size_t k = sep_len; k > 0; --k) {
        if (pos + 1 >= sizeof(integer))
          return false;
        integer[pos++] = sep[k - 1];
      }
      in_group = 0;
      if (*next_group != '\0') {
        unsigned char g = static_cast<unsigned char>(*next_group);
        if (g == static_cast<unsigned char>(cpp::numeric_limits<char>::max()))
          grouped = false;
        else {
          group_size = g;
          ++next_group;
        }
      }
    }
    if (pos + 1 >= sizeof(integer))
      return false;
    integer[pos++] = i < int_len ? number[int_len - 1 - i] : spec.fill;
    ++in_group;
  }
  integer[pos] = '\0';
  for (size_t i = 0; i < pos / 2; ++i) {
    char c = integer[i];
    integer[i] = integer[pos - 1 - i];
    integer[pos - 1 - i] = c;
  }

  // The currency symbol and where the sign goes.
  const char *symbol = "";
  if (spec.symbol)
    symbol = spec.international ? lc.int_curr_symbol : lc.currency_symbol;
  const char *sign = negative ? lc.negative_sign : lc.positive_sign;
  if (negative && sign[0] == '\0')
    sign = "-";

  // The number is the integer part, the point and the fraction.
  const char *fraction = number + int_len;
  if (*fraction == '.')
    ++fraction;

  if (negative && spec.parentheses)
    out.put('(');
  else if (!spec.parentheses)
    out.put(sign);
  out.put(symbol);
  out.put(integer);
  if (digits > 0) {
    out.put(point);
    out.put(fraction);
  }
  if (negative && spec.parentheses)
    out.put(')');
  return true;
}

// strfmon and strfmon_l: the formatted amounts and the text around them.
LIBC_INLINE ssize_t strfmon(char *__restrict s, size_t maxsize,
                            const char *__restrict format, va_list args) {
  const struct lconv &lc = *internal::localeconv_impl();
  Output out(s, maxsize);
  for (const char *p = format; *p != '\0'; ++p) {
    if (*p != '%') {
      out.put(*p);
      continue;
    }
    ++p;
    if (*p == '%') {
      out.put('%');
      continue;
    }
    Spec spec;
    for (bool flags = true; flags;) {
      switch (*p) {
      case '=':
        if (p[1] == '\0') {
          libc_errno = EINVAL;
          return -1;
        }
        spec.fill = p[1];
        p += 2;
        break;
      case '^':
        spec.group = false;
        ++p;
        break;
      case '+':
        spec.parentheses = false;
        ++p;
        break;
      case '(':
        spec.parentheses = true;
        ++p;
        break;
      case '!':
        spec.symbol = false;
        ++p;
        break;
      case '-':
        spec.left_justify = true;
        ++p;
        break;
      default:
        flags = false;
        break;
      }
    }
    int number;
    if (parse_number(p, number))
      spec.width = number;
    if (*p == '#') {
      ++p;
      if (!parse_number(p, number)) {
        libc_errno = EINVAL;
        return -1;
      }
      spec.left_precision = number;
    }
    if (*p == '.') {
      ++p;
      if (!parse_number(p, number)) {
        libc_errno = EINVAL;
        return -1;
      }
      spec.right_precision = number;
    }
    if (*p == 'i')
      spec.international = true;
    else if (*p != 'n') {
      libc_errno = EINVAL;
      return -1;
    }

    const double value = va_arg(args, double);

    // Format it into a scratch buffer to know how wide it is.
    char scratch[2048];
    Output piece(scratch, sizeof(scratch));
    if (!format_amount(spec, value, lc, piece) || !piece.fits()) {
      libc_errno = E2BIG;
      return -1;
    }
    piece.finish();
    const size_t len = piece.count();
    const size_t pad = spec.width > 0 && static_cast<size_t>(spec.width) > len
                           ? static_cast<size_t>(spec.width) - len
                           : 0;
    if (!spec.left_justify)
      out.put_n(' ', pad);
    out.put(scratch);
    if (spec.left_justify)
      out.put_n(' ', pad);
  }
  if (!out.fits()) {
    libc_errno = E2BIG;
    return -1;
  }
  out.finish();
  return static_cast<ssize_t>(out.count());
}

} // namespace monetary
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_MONETARY_STRFMON_IMPL_H
