//===-- Test that exit flushes what was written to stdout -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/fflush.h"
#include "src/stdio/fputs.h"
#include "src/stdio/stdout.h"
#include "src/stdlib/exit.h"
#include "src/string/strncmp.h"
#include "src/sys/wait/waitpid.h"
#include "src/unistd/close.h"
#include "src/unistd/dup2.h"
#include "src/unistd/fork.h"
#include "src/unistd/pipe.h"
#include "src/unistd/read.h"

#include "test/IntegrationTest/test.h"

// The standard streams were opened by nobody and so never join the list of
// open files. Flushing that list does not reach them, which is what used to
// lose whatever was written to stdout without a newline after it.
TEST_MAIN([[maybe_unused]] int argc, [[maybe_unused]] char **argv,
          [[maybe_unused]] char **envp) {
  constexpr const char *TAIL = "stdout tail with no newline";
  constexpr size_t TAIL_LENGTH = 27;

  int fds[2];
  ASSERT_EQ(LIBC_NAMESPACE::pipe(fds), 0);

  // Whatever the parent has written so far is in the same buffer the child
  // will inherit, and would come down the pipe as well.
  LIBC_NAMESPACE::fflush(LIBC_NAMESPACE::stdout);

  pid_t child = LIBC_NAMESPACE::fork();
  ASSERT_TRUE(child >= 0);
  if (child == 0) {
    // Nothing here reports anything, since the only way out is the pipe and
    // what is on it is what the test is about.
    LIBC_NAMESPACE::close(fds[0]);
    if (LIBC_NAMESPACE::dup2(fds[1], 1) < 0)
      LIBC_NAMESPACE::exit(1);
    LIBC_NAMESPACE::close(fds[1]);
    // Nothing but the flush at exit can get this out: it is buffered, and a
    // pipe is not a terminal.
    LIBC_NAMESPACE::fputs(TAIL, LIBC_NAMESPACE::stdout);
    LIBC_NAMESPACE::exit(0);
  }

  LIBC_NAMESPACE::close(fds[1]);
  char buffer[64] = {};
  ssize_t taken = LIBC_NAMESPACE::read(fds[0], buffer, sizeof(buffer) - 1);
  LIBC_NAMESPACE::close(fds[0]);

  int status = 0;
  ASSERT_EQ(LIBC_NAMESPACE::waitpid(child, &status, 0), child);

  ASSERT_EQ(taken, static_cast<ssize_t>(TAIL_LENGTH));
  ASSERT_EQ(LIBC_NAMESPACE::strncmp(buffer, TAIL, TAIL_LENGTH), 0);
  return 0;
}
