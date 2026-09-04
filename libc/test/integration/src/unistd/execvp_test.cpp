//===-- Integration test for execvp ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/setenv.h"
#include "src/sys/wait/waitpid.h"
#include "src/unistd/execvp.h"
#include "src/unistd/fork.h"
#include "test/IntegrationTest/test.h"

#include <errno.h>
#include <sys/wait.h>

// A name with no slash is looked up in PATH. The test binary is run from the
// directory holding the helper, so "." finding it proves the search works.
void execvp_finds_name_on_path() {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("PATH", ".", 1), 0);
  pid_t pid = LIBC_NAMESPACE::fork();
  if (pid == 0) {
    char *const argv[] = {
        const_cast<char *>("execv_test_normal_exit"),
        nullptr,
    };
    LIBC_NAMESPACE::execvp("libc_execv_test_normal_exit", argv);
    // Only reached if the search failed.
    __builtin_trap();
  }
  ASSERT_TRUE(pid > 0);
  int status;
  pid_t cpid = LIBC_NAMESPACE::waitpid(pid, &status, 0);
  ASSERT_EQ(cpid, pid);
  ASSERT_TRUE(WIFEXITED(status));
}

// A name containing a slash is used directly, with no search.
void execvp_uses_path_with_slash_directly() {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("PATH", "/nonexistent", 1), 0);
  pid_t pid = LIBC_NAMESPACE::fork();
  if (pid == 0) {
    char *const argv[] = {
        const_cast<char *>("execv_test_normal_exit"),
        nullptr,
    };
    LIBC_NAMESPACE::execvp("./libc_execv_test_normal_exit", argv);
    __builtin_trap();
  }
  ASSERT_TRUE(pid > 0);
  int status;
  pid_t cpid = LIBC_NAMESPACE::waitpid(pid, &status, 0);
  ASSERT_EQ(cpid, pid);
  ASSERT_TRUE(WIFEXITED(status));
}

void execvp_reports_missing_file() {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("PATH", ".", 1), 0);
  char *const argv[] = {const_cast<char *>("nope"), nullptr};
  ASSERT_EQ(LIBC_NAMESPACE::execvp("libc-definitely-not-here", argv), -1);
  ASSERT_ERRNO_EQ(ENOENT);
  errno = 0;
}

void execvp_rejects_empty_name() {
  char *const argv[] = {const_cast<char *>(""), nullptr};
  ASSERT_EQ(LIBC_NAMESPACE::execvp("", argv), -1);
  ASSERT_ERRNO_EQ(ENOENT);
  errno = 0;

  ASSERT_EQ(LIBC_NAMESPACE::execvp(nullptr, argv), -1);
  ASSERT_ERRNO_EQ(ENOENT);
  errno = 0;
}

TEST_MAIN([[maybe_unused]] int argc, [[maybe_unused]] char **argv,
          [[maybe_unused]] char **envp) {
  execvp_rejects_empty_name();
  execvp_reports_missing_file();
  execvp_finds_name_on_path();
  execvp_uses_path_with_slash_directly();
  return 0;
}
