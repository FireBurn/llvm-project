//===-- Integration test for popen and pclose -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "src/fcntl/fcntl.h"
#include "src/stdio/fgets.h"
#include "src/stdio/fileno.h"
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
  // A direction cannot be contradicted, and there has to be one.
  ASSERT_TRUE(LIBC_NAMESPACE::popen("true", "rw") == nullptr);
  ASSERT_ERRNO_EQ(EINVAL);
  errno = 0;
  ASSERT_TRUE(LIBC_NAMESPACE::popen("true", "e") == nullptr);
  ASSERT_ERRNO_EQ(EINVAL);
  errno = 0;
  ASSERT_TRUE(LIBC_NAMESPACE::popen("true", "") == nullptr);
  ASSERT_ERRNO_EQ(EINVAL);
  errno = 0;
  ASSERT_TRUE(LIBC_NAMESPACE::popen("true", "x") == nullptr);
  ASSERT_ERRNO_EQ(EINVAL);
  errno = 0;
}

// The 'e' the other libcs take asks for the stream to be closed when
// something is executed. It may appear anywhere in the mode.
void closes_on_exec_when_asked() {
  ::FILE *pipe = LIBC_NAMESPACE::popen("true", "re");
  ASSERT_TRUE(pipe != nullptr);
  int flags = LIBC_NAMESPACE::fcntl(LIBC_NAMESPACE::fileno(pipe), F_GETFD);
  ASSERT_TRUE((flags & FD_CLOEXEC) != 0);
  ASSERT_TRUE(LIBC_NAMESPACE::pclose(pipe) >= 0);

  // And without it the stream stays open across one.
  pipe = LIBC_NAMESPACE::popen("true", "r");
  ASSERT_TRUE(pipe != nullptr);
  flags = LIBC_NAMESPACE::fcntl(LIBC_NAMESPACE::fileno(pipe), F_GETFD);
  ASSERT_TRUE((flags & FD_CLOEXEC) == 0);
  ASSERT_TRUE(LIBC_NAMESPACE::pclose(pipe) >= 0);

  // The direction may be repeated, and 'e' may come first.
  pipe = LIBC_NAMESPACE::popen("true", "err");
  ASSERT_TRUE(pipe != nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::pclose(pipe) >= 0);
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
  closes_on_exec_when_asked();
  return 0;
}
