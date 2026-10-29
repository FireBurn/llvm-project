//===-- Test of the loader run as a command ---------------------------   ===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that the loader can be run as a command, `ld.so program args`,
/// where it used to jump back to its own entry point for ever. The program
/// returns nonzero if any check fails.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

extern "C" int main() {
  pid_t pid = fork();
  CHECK(pid >= 0);
  if (pid < 0)
    return 1;
  if (pid == 0) {
    char *args[] = {const_cast<char *>(LIBC_TEST_LOADER),
                    const_cast<char *>(LIBC_TEST_LOADER_CHILD),
                    const_cast<char *>("first"), const_cast<char *>("second"),
                    nullptr};
    char *env[] = {nullptr};
    execve(LIBC_TEST_LOADER, args, env);
    _exit(127);
  }

  // A loader which loops would never end, so it is given a little time.
  int status = -1;
  for (int waited = 0; waited < 100; ++waited) {
    pid_t done = waitpid(pid, &status, WNOHANG);
    if (done == pid)
      break;
    usleep(100000);
    if (waited == 99) {
      kill(pid, SIGKILL);
      waitpid(pid, &status, 0);
      CHECK(!"the loader did not finish");
    }
  }
  CHECK(WIFEXITED(status));
  CHECK(WEXITSTATUS(status) == 0);
  return failures == 0 ? 0 : 1;
}
