//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration tests for daemon.
///
//===----------------------------------------------------------------------===//

#include "src/string/strcmp.h"
#include "src/sys/wait/waitpid.h"
#include "src/unistd/_exit.h"
#include "src/unistd/close.h"
#include "src/unistd/daemon.h"
#include "src/unistd/fork.h"
#include "src/unistd/getcwd.h"
#include "src/unistd/getpid.h"
#include "src/unistd/getsid.h"
#include "src/unistd/pipe.h"
#include "src/unistd/read.h"
#include "src/unistd/write.h"

#include "test/IntegrationTest/test.h"

#include <sys/wait.h>
#include <unistd.h>

// daemon returns in the grandchild, which has to report through a pipe that
// stays open because noclose is asked for. The process which called it goes
// away with status 0.
TEST_MAIN() {
  int fds[2];
  ASSERT_EQ(LIBC_NAMESPACE::pipe(fds), 0);

  pid_t middle = LIBC_NAMESPACE::fork();
  ASSERT_TRUE(middle >= 0);
  if (middle == 0) {
    if (LIBC_NAMESPACE::daemon(0, 1) != 0)
      LIBC_NAMESPACE::_exit(2);
    char report[2];
    // A session leader has its own process id as its session id.
    report[0] = LIBC_NAMESPACE::getsid(0) == LIBC_NAMESPACE::getpid() ? 'S' : 's';
    char cwd[8] = {};
    report[1] = (LIBC_NAMESPACE::getcwd(cwd, sizeof(cwd)) != nullptr &&
                 LIBC_NAMESPACE::strcmp(cwd, "/") == 0)
                    ? '/'
                    : '?';
    LIBC_NAMESPACE::write(fds[1], report, sizeof(report));
    LIBC_NAMESPACE::_exit(0);
  }

  int status = 0;
  ASSERT_EQ(LIBC_NAMESPACE::waitpid(middle, &status, 0), middle);
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 0);

  ASSERT_EQ(LIBC_NAMESPACE::close(fds[1]), 0);
  char report[2] = {};
  ASSERT_EQ(LIBC_NAMESPACE::read(fds[0], report, sizeof(report)), ssize_t(2));
  ASSERT_EQ(report[0], 'S');
  ASSERT_EQ(report[1], '/');
  ASSERT_EQ(LIBC_NAMESPACE::close(fds[0]), 0);
  return 0;
}
