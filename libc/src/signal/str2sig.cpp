//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of str2sig.
///
//===----------------------------------------------------------------------===//

#include "src/signal/str2sig.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/signal/signal_names.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// Reads a decimal number which fills the whole text. A distance beyond the
// range of signals is as wrong as any other, so it saturates.
bool parse_number(cpp::string_view text, int &value) {
  if (text.empty() || text.size() > 9)
    return false;
  value = 0;
  for (char c : text) {
    if (c < '0' || c > '9')
      return false;
    value = value * 10 + (c - '0');
  }
  return true;
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(int, str2sig, (const char *str, int *pnum)) {
  LIBC_CRASH_ON_NULLPTR(str);
  LIBC_CRASH_ON_NULLPTR(pnum);
  cpp::string_view text(str);

  int number;
  if (parse_number(text, number)) {
    if (number >= NSIG)
      return -1;
    *pnum = number;
    return 0;
  }

  for (const signal_names::Name &entry : signal_names::NAMES) {
    if (text == entry.name) {
      *pnum = entry.number;
      return 0;
    }
  }
  for (const signal_names::Name &entry : signal_names::ALIASES) {
    if (text == entry.name) {
      *pnum = entry.number;
      return 0;
    }
  }

  // RTMIN, RTMAX, RTMIN+n and RTMAX-n, with n leaving the result inside
  // the range.
  if (text.starts_with("RTM")) {
    cpp::string_view rest = text.substr(3);
    int base = 0;
    int sign = 0;
    if (rest.starts_with("IN")) {
      base = SIGRTMIN;
      sign = 1;
    } else if (rest.starts_with("AX")) {
      base = SIGRTMAX;
      sign = -1;
    } else {
      return -1;
    }
    rest = rest.substr(2);
    if (rest.empty()) {
      *pnum = base;
      return 0;
    }
    if (rest[0] != (sign > 0 ? '+' : '-'))
      return -1;
    int distance;
    if (!parse_number(rest.substr(1), distance))
      return -1;
    int result = base + sign * distance;
    if (distance == 0 || result < SIGRTMIN || result > SIGRTMAX)
      return -1;
    *pnum = result;
    return 0;
  }
  return -1;
}

} // namespace LIBC_NAMESPACE_DECL
