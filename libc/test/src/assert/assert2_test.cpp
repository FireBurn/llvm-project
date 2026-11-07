//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for __assert2 and __assert, which bionic's assert and code
/// generated for Android call.
///
//===----------------------------------------------------------------------===//

#include "hdr/signal_macros.h"
#include "src/assert/__assert.h"
#include "src/assert/__assert2.h"
#include "src/unistd/close.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcAssert2, FailAborts) {
  // Close standard error for the child process so we don't print the message.
  EXPECT_DEATH(
      [] {
        LIBC_NAMESPACE::close(2);
        LIBC_NAMESPACE::__assert2("file.c", 12, "function", "bad access");
      },
      WITH_SIGNAL(SIGABRT));
}

TEST(LlvmLibcAssert2, FunctionlessFormAborts) {
  EXPECT_DEATH(
      [] {
        LIBC_NAMESPACE::close(2);
        LIBC_NAMESPACE::__assert("file.c", 12, "bad access");
      },
      WITH_SIGNAL(SIGABRT));
}
