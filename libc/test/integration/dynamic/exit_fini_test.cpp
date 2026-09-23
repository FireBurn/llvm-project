//===-- Test of finalisers run at exit ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that the finalisers of a shared object run when the program exits,
/// both for one loaded at startup and for one opened with dlopen and never
/// closed. The program forks, and the child reports through a pipe, since
/// nothing can be looked at once a process has gone. The program returns
/// nonzero if any check fails.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <dlfcn.h>
#include <sys/wait.h>
#include <unistd.h>

extern "C" int llvm_libc_exit_fd = -1;

extern "C" int main() {
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
    llvm_libc_exit_fd = fds[1];
#ifndef LIBC_TEST_LINKED
    // Opened and left open, so that exit is what finalises it.
    if (dlopen(LIBC_TEST_DL_EXIT_OBJECT, RTLD_NOW) == nullptr)
      _exit(2);
#endif
    // Returning from main is what leads to the finalisers.
    return 0;
  }

  close(fds[1]);
  char byte = 0;
  long got = read(fds[0], &byte, 1);
  CHECK(got == 1);
  CHECK(byte == 'x');
  // Once, and not again.
  CHECK(read(fds[0], &byte, 1) == 0);
  close(fds[0]);

  int status = -1;
  CHECK(waitpid(pid, &status, 0) == pid);
  CHECK(status == 0);
  return failures == 0 ? 0 : 1;
}
