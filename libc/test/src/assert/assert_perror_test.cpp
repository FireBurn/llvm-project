//===-- Unittests for assert_perror -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for assert_perror.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "src/assert/__assert_perror_fail.h"
#include "src/unistd/close.h"
#include "test/UnitTest/Test.h"

// assert_perror(errnum) expands to a call of __assert_perror_fail when the
// error number is not zero.
TEST(LlvmLibcAssertPerror, FailAborts) {
  // Close standard error for the child process so we don't print the message.
  EXPECT_DEATH(
      [] {
        LIBC_NAMESPACE::close(2);
        LIBC_NAMESPACE::__assert_perror_fail(ENOENT, "file.c", 12, "function");
      },
      WITH_SIGNAL(SIGABRT));
}
