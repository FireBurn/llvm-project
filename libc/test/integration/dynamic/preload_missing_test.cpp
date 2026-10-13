//===-- Test of LD_PRELOAD with a missing library -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that an object named in LD_PRELOAD whose own libraries are missing is
/// ignored with a message, as glibc does, and the program still starts. The
/// program runs itself under such a preload and looks at what it said. It
/// returns nonzero if any check fails.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

extern "C" int main(int argc, char **) {
  // The program run under the preload has nothing to do but start.
  if (argc > 1)
    return 0;

  int fds[2];
  CHECK(pipe(fds) == 0);
  if (failures)
    return 1;

  pid_t pid = fork();
  CHECK(pid >= 0);
  if (pid < 0)
    return 1;
  if (pid == 0) {
    close(fds[0]);
    dup2(fds[1], 2);
    char *args[] = {const_cast<char *>("preload_missing_test"),
                    const_cast<char *>("child"), nullptr};
    char *env[] = {const_cast<char *>("LD_PRELOAD=" LIBC_TEST_DL_MISSING_OBJECT),
                   nullptr};
    execve("/proc/self/exe", args, env);
    _exit(127);
  }

  close(fds[1]);
  char said[512];
  size_t filled = 0;
  for (;;) {
    long got = read(fds[0], said + filled, sizeof(said) - 1 - filled);
    if (got <= 0)
      break;
    filled += static_cast<size_t>(got);
  }
  said[filled] = '\0';
  close(fds[0]);

  int status = -1;
  CHECK(waitpid(pid, &status, 0) == pid);
  CHECK(status == 0);
  CHECK(strstr(said, "from LD_PRELOAD cannot be preloaded") != nullptr);
  return failures == 0 ? 0 : 1;
}
