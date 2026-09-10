//===-- Unittests for vfork -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/wait/waitpid.h"
#include "src/unistd/_exit.h"
#include "src/unistd/getpid.h"
#include "src/unistd/vfork.h"

#include "test/IntegrationTest/test.h"

#include <sys/wait.h>
#include <unistd.h>

// Between vfork and the exit of the child, the child may do no more than end
// itself or hand the process over to another program, so each of these tests
// ends its child with _exit.

void vfork_and_exit_with_status() {
  pid_t pid = LIBC_NAMESPACE::vfork();
  if (pid == 0)
    LIBC_NAMESPACE::_exit(42);
  ASSERT_TRUE(pid > 0);

  int status;
  pid_t cpid = LIBC_NAMESPACE::waitpid(pid, &status, 0);
  ASSERT_EQ(cpid, pid);
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 42);
}

void child_is_a_new_process() {
  pid_t parent = LIBC_NAMESPACE::getpid();

  pid_t pid = LIBC_NAMESPACE::vfork();
  if (pid == 0) {
    // The child reports whether it is a process of its own by the only means
    // it has, which is the status it exits with.
    LIBC_NAMESPACE::_exit(LIBC_NAMESPACE::getpid() == parent ? 1 : 0);
  }
  ASSERT_TRUE(pid > 0);

  int status;
  pid_t cpid = LIBC_NAMESPACE::waitpid(pid, &status, 0);
  ASSERT_EQ(cpid, pid);
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 0);
}

TEST_MAIN() {
  vfork_and_exit_with_status();
  child_is_a_new_process();
  return 0;
}
