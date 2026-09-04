//===-- Integration test for execlp ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/setenv.h"
#include "src/sys/wait/waitpid.h"
#include "src/unistd/execlp.h"
#include "src/unistd/fork.h"
#include "test/IntegrationTest/test.h"

#include <errno.h>
#include <sys/wait.h>

// The arguments are given one at a time rather than in an array, and the
// name with no slash is looked up in PATH as execvp does it.
void execlp_finds_name_on_path() {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("PATH", ".", 1), 0);
  pid_t pid = LIBC_NAMESPACE::fork();
  if (pid == 0) {
    LIBC_NAMESPACE::execlp("libc_execv_test_normal_exit",
                           "execv_test_normal_exit", nullptr);
    // Only reached if the search failed.
    __builtin_trap();
  }
  ASSERT_TRUE(pid > 0);
  int status;
  pid_t cpid = LIBC_NAMESPACE::waitpid(pid, &status, 0);
  ASSERT_EQ(cpid, pid);
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 0);
}

void execlp_reports_missing_file() {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("PATH", ".", 1), 0);
  ASSERT_EQ(LIBC_NAMESPACE::execlp("libc-definitely-not-here", "nope", nullptr),
            -1);
  ASSERT_ERRNO_EQ(ENOENT);
  errno = 0;
}

TEST_MAIN([[maybe_unused]] int argc, [[maybe_unused]] char **argv,
          [[maybe_unused]] char **envp) {
  execlp_reports_missing_file();
  execlp_finds_name_on_path();
  return 0;
}
