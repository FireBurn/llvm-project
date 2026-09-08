//===-- Unittests for psignal ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/signal_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/StringUtil/signal_to_string.h"
#include "src/signal/psignal.h"
#include "src/unistd/close.h"
#include "src/unistd/dup.h"
#include "src/unistd/dup2.h"
#include "src/unistd/pipe.h"
#include "src/unistd/read.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::cpp::string_view;

namespace {

// Runs psignal with standard error pointed into a pipe, and hands back what
// came out of the other end.
string_view capture(int sig, const char *prefix, char *buffer, size_t size) {
  int fds[2];
  if (LIBC_NAMESPACE::pipe(fds) != 0)
    return string_view();
  int saved = LIBC_NAMESPACE::dup(2);
  LIBC_NAMESPACE::dup2(fds[1], 2);
  LIBC_NAMESPACE::psignal(sig, prefix);
  LIBC_NAMESPACE::dup2(saved, 2);
  LIBC_NAMESPACE::close(saved);
  LIBC_NAMESPACE::close(fds[1]);
  ssize_t got = LIBC_NAMESPACE::read(fds[0], buffer, size);
  LIBC_NAMESPACE::close(fds[0]);
  return string_view(buffer, got < 0 ? 0 : static_cast<size_t>(got));
}

} // anonymous namespace

// The prefix goes first with a colon after it, then what the signal is,
// then the end of the line.
TEST(LlvmLibcPsignalTest, PutsThePrefixBeforeTheDescription) {
  char buffer[128];
  string_view description = LIBC_NAMESPACE::get_signal_string(SIGTERM);
  string_view out = capture(SIGTERM, "llvm-libc", buffer, sizeof(buffer));

  ASSERT_TRUE(out.starts_with("llvm-libc: "));
  out.remove_prefix(11);
  ASSERT_TRUE(out.starts_with(description));
  out.remove_prefix(description.size());
  ASSERT_TRUE(out == string_view("\n"));
}

// With no prefix, or an empty one, the description stands on its own.
TEST(LlvmLibcPsignalTest, WithoutAPrefixTheDescriptionStandsAlone) {
  string_view description = LIBC_NAMESPACE::get_signal_string(SIGINT);
  const char *prefixes[] = {nullptr, ""};
  for (const char *prefix : prefixes) {
    char buffer[128];
    string_view out = capture(SIGINT, prefix, buffer, sizeof(buffer));
    ASSERT_TRUE(out.starts_with(description));
    out.remove_prefix(description.size());
    ASSERT_TRUE(out == string_view("\n"));
  }
}
