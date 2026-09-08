//===-- Integration test for execvpe --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/setenv.h"
#include "src/sys/wait/waitpid.h"
#include "src/unistd/execvpe.h"
#include "src/unistd/fork.h"
#include "test/IntegrationTest/test.h"

#include <errno.h>
#include <sys/wait.h>

// The search is execvp's, but the environment is the caller's rather than
// the one this process holds. The helper raises a signal when it cannot find
// EXECV_TEST, so an exit rather than a signal says the environment arrived.
void execvpe_passes_the_given_environment() {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("PATH", ".", 1), 0);
  pid_t pid = LIBC_NAMESPACE::fork();
  if (pid == 0) {
    char *const child_argv[] = {
        const_cast<char *>("execv_test_normal_exit"),
        nullptr,
    };
    char *const child_envp[] = {
        const_cast<char *>("EXECV_TEST=PASS"),
        nullptr,
    };
    LIBC_NAMESPACE::execvpe("libc_execv_test_normal_exit", child_argv,
                            child_envp);
    __builtin_trap();
  }
  ASSERT_TRUE(pid > 0);
  int status;
  pid_t cpid = LIBC_NAMESPACE::waitpid(pid, &status, 0);
  ASSERT_EQ(cpid, pid);
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 0);
}

// An empty environment reaches the child as one, and the helper says so by
// raising a signal rather than exiting.
void execvpe_passes_an_empty_environment() {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("PATH", ".", 1), 0);
  pid_t pid = LIBC_NAMESPACE::fork();
  if (pid == 0) {
    char *const child_argv[] = {
        const_cast<char *>("execv_test_normal_exit"),
        nullptr,
    };
    char *const child_envp[] = {nullptr};
    LIBC_NAMESPACE::execvpe("libc_execv_test_normal_exit", child_argv,
                            child_envp);
    __builtin_trap();
  }
  ASSERT_TRUE(pid > 0);
  int status;
  pid_t cpid = LIBC_NAMESPACE::waitpid(pid, &status, 0);
  ASSERT_EQ(cpid, pid);
  ASSERT_TRUE(WIFSIGNALED(status));
}

void execvpe_reports_missing_file() {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("PATH", ".", 1), 0);
  char *const bad_argv[] = {const_cast<char *>("nope"), nullptr};
  char *const bad_envp[] = {nullptr};
  ASSERT_EQ(
      LIBC_NAMESPACE::execvpe("libc-definitely-not-here", bad_argv, bad_envp),
      -1);
  ASSERT_ERRNO_EQ(ENOENT);
  errno = 0;
}

TEST_MAIN([[maybe_unused]] int argc, [[maybe_unused]] char **argv,
          [[maybe_unused]] char **envp) {
  execvpe_reports_missing_file();
  execvpe_passes_the_given_environment();
  execvpe_passes_an_empty_environment();
  return 0;
}
