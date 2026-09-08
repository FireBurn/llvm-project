//===-- Implementation of psignal -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/signal/psignal.h"

#include "hdr/types/FILE.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/File/file.h"
#include "src/__support/StringUtil/signal_to_string.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdio/stderr.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

int write_out(cpp::string_view text) {
  if (text.empty())
    return 0;
  auto result = reinterpret_cast<LIBC_NAMESPACE::File *>(LIBC_NAMESPACE::stderr)
                    ->write_unlocked(text.data(), text.size());
  if (result.has_error())
    return result.error;
  return 0;
}

} // anonymous namespace

// Says what a signal was, the way perror says what an error was. Where `s`
// names something it goes first, then a colon; where it is null or empty the
// description stands on its own.
LLVM_LIBC_FUNCTION(void, psignal, (int sig, const char *s)) {
  cpp::string_view prefix(s == nullptr ? "" : s);

  int err = 0;
  if (!prefix.empty()) {
    err = write_out(prefix);
    if (err == 0)
      err = write_out(": ");
  }
  if (err == 0)
    err = write_out(get_signal_string(sig));
  if (err == 0)
    err = write_out("\n");

  if (err != 0)
    libc_errno = err;
}

} // namespace LIBC_NAMESPACE_DECL
