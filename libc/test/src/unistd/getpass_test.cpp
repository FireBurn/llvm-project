//===-- Unittests for getpass ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/termios_macros.h"
#include "hdr/types/struct_termios.h"
#include "src/__support/libc_errno.h"
#include "src/pty/forkpty.h"
#include "src/stdlib/_Exit.h"
#include "src/string/strcmp.h"
#include "src/sys/wait/waitpid.h"
#include "src/termios/tcgetattr.h"
#include "src/unistd/close.h"
#include "src/unistd/getpass.h"
#include "src/unistd/usleep.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

#include <sys/wait.h>

using LlvmLibcGetpassTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {

// getpass throws away whatever was typed ahead of the prompt, so the answer
// cannot be written until the child has the terminal quiet. Turning the echo
// off is the last thing it does before reading, and the setting belongs to
// the pair rather than to either end, so the parent can watch for it.
bool terminal_went_quiet(int primary) {
  for (int tries = 0; tries < 2000; ++tries) {
    struct termios settings;
    if (LIBC_NAMESPACE::tcgetattr(primary, &settings) == 0 &&
        (settings.c_lflag & ECHO) == 0)
      return true;
    LIBC_NAMESPACE::usleep(1000);
  }
  return false;
}

} // anonymous namespace

// getpass reads from the terminal the session is attached to rather than
// from standard input, so the answer has to be typed at one. forkpty gives
// the child a terminal of its own, and the parent types into it.
TEST_F(LlvmLibcGetpassTest, ReadsWhatWasTypedAtTheTerminal) {
  constexpr const char *ANSWER = "a secret";

  int primary = -1;
  pid_t pid = LIBC_NAMESPACE::forkpty(&primary, nullptr, nullptr, nullptr);
  if (pid < 0) {
    // A machine need not have terminals to hand out at all.
    LIBC_NAMESPACE::libc_errno = 0;
    return;
  }

  if (pid == 0) {
    const char *read = LIBC_NAMESPACE::getpass("Password: ");
    int code =
        read != nullptr && LIBC_NAMESPACE::strcmp(read, ANSWER) == 0 ? 0 : 1;
    LIBC_NAMESPACE::_Exit(code);
  }

  ASSERT_GT(primary, 0);
  const bool quiet = terminal_went_quiet(primary);
  if (quiet)
    ASSERT_EQ(LIBC_NAMESPACE::write(primary, "a secret\n", 9),
              static_cast<ssize_t>(9));

  if (!quiet) {
    // Letting go of the last handle on the pair hangs the session up, which
    // ends the read the child is in. A terminal that never went quiet costs
    // a skipped check rather than a wait that never ends.
    ASSERT_EQ(LIBC_NAMESPACE::close(primary), 0);
    int hung_up = 0;
    ASSERT_EQ(LIBC_NAMESPACE::waitpid(pid, &hung_up, 0), pid);
    return;
  }

  int status = 0;
  ASSERT_EQ(LIBC_NAMESPACE::waitpid(pid, &status, 0), pid);
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 0);

  ASSERT_EQ(LIBC_NAMESPACE::close(primary), 0);
}

// A terminal that is closed before anything is typed ends the read, and
// nothing is reported back rather than an empty answer.
TEST_F(LlvmLibcGetpassTest, NothingIsTyped) {
  int primary = -1;
  pid_t pid = LIBC_NAMESPACE::forkpty(&primary, nullptr, nullptr, nullptr);
  if (pid < 0) {
    LIBC_NAMESPACE::libc_errno = 0;
    return;
  }

  if (pid == 0) {
    const char *read = LIBC_NAMESPACE::getpass("Password: ");
    // Either nothing at all or an empty line is a fair answer to a terminal
    // that said nothing; what must not happen is a wait that never ends.
    LIBC_NAMESPACE::_Exit(read == nullptr || read[0] == '\0' ? 0 : 1);
  }

  ASSERT_GT(primary, 0);
  ASSERT_EQ(LIBC_NAMESPACE::close(primary), 0);

  // Losing the terminal hangs up the session it belonged to, so the child
  // is as likely to be cut off as to answer. What is being checked is that
  // it ends at all rather than waiting for a line that can never arrive.
  int status = 0;
  ASSERT_EQ(LIBC_NAMESPACE::waitpid(pid, &status, 0), pid);
}
