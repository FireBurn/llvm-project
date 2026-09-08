//===-- Unittests for openpty, forkpty and login_tty ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/types/struct_termios.h"
#include "hdr/types/struct_winsize.h"
#include "src/__support/libc_errno.h"
#include "src/pty/forkpty.h"
#include "src/pty/login_tty.h"
#include "src/pty/openpty.h"
#include "src/stdlib/_Exit.h"
#include "src/string/memcmp.h"
#include "src/string/strlen.h"
#include "src/sys/wait/waitpid.h"
#include "src/termios/tcgetattr.h"
#include "src/unistd/close.h"
#include "src/unistd/isatty.h"
#include "src/unistd/read.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <sys/wait.h>

using LlvmLibcPtyTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// The pair is joined, so what is written to one end is read from the other.
TEST_F(LlvmLibcPtyTest, OpensAJoinedPair) {
  int primary = -1;
  int secondary = -1;
  char name[256] = {};

  if (LIBC_NAMESPACE::openpty(&primary, &secondary, name, nullptr, nullptr) !=
      0) {
    // A machine need not have terminals to hand out at all.
    LIBC_NAMESPACE::libc_errno = 0;
    return;
  }
  ASSERT_GT(primary, 0);
  ASSERT_GT(secondary, 0);
  ASSERT_GT(LIBC_NAMESPACE::strlen(name), size_t(0));

  // Both ends are terminals, and the second has terminal settings.
  ASSERT_EQ(LIBC_NAMESPACE::isatty(secondary), 1);
  struct termios settings;
  ASSERT_THAT(LIBC_NAMESPACE::tcgetattr(secondary, &settings), Succeeds(0));

  constexpr char TEXT[] = "through the pair";
  constexpr size_t SIZE = sizeof(TEXT) - 1;
  ASSERT_THAT(LIBC_NAMESPACE::write(primary, TEXT, SIZE),
              Succeeds<ssize_t>(SIZE));

  char buf[SIZE];
  ASSERT_THAT(LIBC_NAMESPACE::read(secondary, buf, SIZE),
              Succeeds<ssize_t>(SIZE));
  ASSERT_EQ(LIBC_NAMESPACE::memcmp(buf, TEXT, SIZE), 0);

  ASSERT_THAT(LIBC_NAMESPACE::close(secondary), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(primary), Succeeds(0));
}

// forkpty starts a child holding the second end as its standard streams and
// as the terminal of a session of its own, which is login_tty's work.
TEST_F(LlvmLibcPtyTest, StartsAChildOnTheOtherEnd) {
  int primary = -1;
  pid_t pid = LIBC_NAMESPACE::forkpty(&primary, nullptr, nullptr, nullptr);
  if (pid < 0) {
    LIBC_NAMESPACE::libc_errno = 0;
    return;
  }

  if (pid == 0) {
    // The child's standard streams are the terminal, and it is the one the
    // child's session is attached to.
    int code = LIBC_NAMESPACE::isatty(0) == 1 && LIBC_NAMESPACE::isatty(1) == 1
                   ? 0
                   : 1;
    LIBC_NAMESPACE::_Exit(code);
  }

  ASSERT_GT(primary, 0);
  int status = 0;
  ASSERT_EQ(LIBC_NAMESPACE::waitpid(pid, &status, 0), pid);
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 0);

  ASSERT_THAT(LIBC_NAMESPACE::close(primary), Succeeds(0));
}

// login_tty puts the descriptor in place as the session's terminal, which
// takes the session with it, so it is only ever called in a child.
TEST_F(LlvmLibcPtyTest, PutsATerminalInPlaceForTheSession) {
  int primary = -1;
  int secondary = -1;
  if (LIBC_NAMESPACE::openpty(&primary, &secondary, nullptr, nullptr,
                              nullptr) != 0) {
    LIBC_NAMESPACE::libc_errno = 0;
    return;
  }

  pid_t pid = LIBC_NAMESPACE::forkpty(nullptr, nullptr, nullptr, nullptr);
  ASSERT_GE(pid, 0);
  if (pid == 0) {
    // Already in a session of its own, so putting the other terminal in
    // place is all that is left to check.
    int code = LIBC_NAMESPACE::login_tty(secondary) == 0 &&
                       LIBC_NAMESPACE::isatty(0) == 1
                   ? 0
                   : 1;
    LIBC_NAMESPACE::_Exit(code);
  }

  int status = 0;
  ASSERT_EQ(LIBC_NAMESPACE::waitpid(pid, &status, 0), pid);
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 0);

  LIBC_NAMESPACE::close(secondary);
  ASSERT_THAT(LIBC_NAMESPACE::close(primary), Succeeds(0));
}

TEST_F(LlvmLibcPtyTest, DescriptorThatIsNotATerminal) {
  ASSERT_THAT(LIBC_NAMESPACE::login_tty(-1), Fails(ENOTTY));
}
