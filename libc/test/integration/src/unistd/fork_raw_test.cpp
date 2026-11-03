//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration tests for _Fork.
///
//===----------------------------------------------------------------------===//

#include "src/sys/wait/waitpid.h"
#include "src/unistd/_Fork.h"
#include "src/unistd/_exit.h"
#include "src/unistd/getpid.h"

#include "test/IntegrationTest/test.h"

#include <sys/wait.h>
#include <unistd.h>

// _Fork makes the child and nothing else: no handlers run, so the child can
// only report through its exit status.
TEST_MAIN() {
  pid_t parent = LIBC_NAMESPACE::getpid();
  pid_t child = LIBC_NAMESPACE::_Fork();
  ASSERT_TRUE(child >= 0);
  if (child == 0) {
    // The child is a different process and knows it.
    LIBC_NAMESPACE::_exit(LIBC_NAMESPACE::getpid() != parent ? 42 : 1);
  }
  ASSERT_TRUE(child != parent);

  int status = 0;
  ASSERT_EQ(LIBC_NAMESPACE::waitpid(child, &status, 0), child);
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 42);
  return 0;
}
