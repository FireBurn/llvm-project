//===-- Integration test for execl ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/wait/waitpid.h"
#include "src/unistd/execl.h"
#include "src/unistd/fork.h"
#include "test/IntegrationTest/test.h"

#include <errno.h>
#include <sys/wait.h>

// The arguments are given one at a time rather than in an array, and the
// path is used as it stands with no search.
void execl_runs_the_named_program() {
  pid_t pid = LIBC_NAMESPACE::fork();
  if (pid == 0) {
    LIBC_NAMESPACE::execl("./libc_execv_test_normal_exit",
                          "execv_test_normal_exit", nullptr);
    // Only reached if the exec failed.
    __builtin_trap();
  }
  ASSERT_TRUE(pid > 0);
  int status;
  pid_t cpid = LIBC_NAMESPACE::waitpid(pid, &status, 0);
  ASSERT_EQ(cpid, pid);
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 0);
}

void execl_reports_missing_file() {
  ASSERT_EQ(
      LIBC_NAMESPACE::execl("./libc-definitely-not-here", "nope", nullptr), -1);
  ASSERT_ERRNO_EQ(ENOENT);
  errno = 0;
}

TEST_MAIN([[maybe_unused]] int argc, [[maybe_unused]] char **argv,
          [[maybe_unused]] char **envp) {
  execl_reports_missing_file();
  execl_runs_the_named_program();
  return 0;
}
