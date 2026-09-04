//===-- Integration test for popen and pclose -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/fgets.h"
#include "src/stdio/fputs.h"
#include "src/stdio/pclose.h"
#include "src/stdio/popen.h"
#include "src/string/strcmp.h"
#include "test/IntegrationTest/test.h"

#include <errno.h>
#include <sys/wait.h>

void reads_command_output() {
  ::FILE *pipe = LIBC_NAMESPACE::popen("echo hello-from-popen", "r");
  ASSERT_TRUE(pipe != nullptr);
  char buf[64] = {};
  ASSERT_TRUE(LIBC_NAMESPACE::fgets(buf, sizeof(buf), pipe) != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::strcmp(buf, "hello-from-popen\n"), 0);
  int status = LIBC_NAMESPACE::pclose(pipe);
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 0);
}

void writes_to_command_input() {
  ::FILE *pipe = LIBC_NAMESPACE::popen("cat > /dev/null", "w");
  ASSERT_TRUE(pipe != nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::fputs("some input\n", pipe) >= 0);
  int status = LIBC_NAMESPACE::pclose(pipe);
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 0);
}

// The child's exit status has to reach the caller, not be swallowed.
void reports_child_exit_status() {
  ::FILE *pipe = LIBC_NAMESPACE::popen("exit 3", "r");
  ASSERT_TRUE(pipe != nullptr);
  int status = LIBC_NAMESPACE::pclose(pipe);
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 3);
}

// POSIX gives 127 for a command that could not be executed.
void reports_missing_command() {
  ::FILE *pipe = LIBC_NAMESPACE::popen("this-command-does-not-exist", "r");
  ASSERT_TRUE(pipe != nullptr);
  int status = LIBC_NAMESPACE::pclose(pipe);
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 127);
}

void rejects_bad_arguments() {
  ASSERT_TRUE(LIBC_NAMESPACE::popen(nullptr, "r") == nullptr);
  ASSERT_ERRNO_EQ(EINVAL);
  errno = 0;
  // Only "r" and "w" are valid.
  ASSERT_TRUE(LIBC_NAMESPACE::popen("true", "rw") == nullptr);
  ASSERT_ERRNO_EQ(EINVAL);
  errno = 0;
  ASSERT_TRUE(LIBC_NAMESPACE::popen("true", "x") == nullptr);
  ASSERT_ERRNO_EQ(EINVAL);
  errno = 0;
}

// pclose on a stream that did not come from popen must fail rather than wait
// on some unrelated process.
void rejects_foreign_stream() {
  ::FILE *pipe = LIBC_NAMESPACE::popen("true", "r");
  ASSERT_TRUE(pipe != nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::pclose(pipe) >= 0);
  // Now the stream is no longer known.
  ASSERT_EQ(LIBC_NAMESPACE::pclose(pipe), -1);
  ASSERT_ERRNO_EQ(EINVAL);
  errno = 0;
}

TEST_MAIN([[maybe_unused]] int argc, [[maybe_unused]] char **argv,
          [[maybe_unused]] char **envp) {
  reads_command_output();
  writes_to_command_input();
  reports_child_exit_status();
  reports_missing_command();
  rejects_bad_arguments();
  rejects_foreign_stream();
  return 0;
}
