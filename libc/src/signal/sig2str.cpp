//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of sig2str.
///
//===----------------------------------------------------------------------===//

#include "src/signal/sig2str.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/common.h"
#include "src/__support/integer_to_string.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/signal/signal_names.h"
#include "src/string/memory_utils/inline_memcpy.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// Appends the decimal digits of a small non-negative number.
size_t append_number(char *out, size_t at, int value) {
  char digits[12];
  size_t count = 0;
  do {
    digits[count++] = static_cast<char>('0' + value % 10);
    value /= 10;
  } while (value != 0);
  while (count > 0)
    out[at++] = digits[--count];
  return at;
}

size_t append_text(char *out, size_t at, const char *text) {
  while (*text != '\0')
    out[at++] = *text++;
  return at;
}

} // anonymous namespace

// The real time signals are named by their distance from the nearer end of
// the range.
LLVM_LIBC_FUNCTION(int, sig2str, (int signum, char *str)) {
  LIBC_CRASH_ON_NULLPTR(str);
  if (signum < 0 || signum >= NSIG)
    return -1;

  for (const signal_names::Name &entry : signal_names::NAMES) {
    if (entry.number == signum) {
      size_t at = append_text(str, 0, entry.name);
      str[at] = '\0';
      return 0;
    }
  }

  if (signum < SIGRTMIN)
    return -1;
  size_t at = 0;
  if (signum == SIGRTMIN || signum == SIGRTMAX) {
    at = append_text(str, 0, signum == SIGRTMIN ? "RTMIN" : "RTMAX");
  } else if (signum - SIGRTMIN <= SIGRTMAX - signum) {
    at = append_text(str, 0, "RTMIN+");
    at = append_number(str, at, signum - SIGRTMIN);
  } else {
    at = append_text(str, 0, "RTMAX-");
    at = append_number(str, at, SIGRTMAX - signum);
  }
  str[at] = '\0';
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
