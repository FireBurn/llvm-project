//===-- Unittests for fileno ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/func/free.h"
#include "src/__support/libc_errno.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fileno.h"
#include "src/stdio/fmemopen.h"
#include "src/stdio/fopen.h"
#include "src/stdio/open_memstream.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcFilenoTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcFilenoTest, AStreamOverAFileHasOne) {
  auto path = libc_make_test_file_path("fileno.test");
  ::FILE *file = LIBC_NAMESPACE::fopen(path, "w");
  ASSERT_TRUE(file != nullptr);
  EXPECT_GT(LIBC_NAMESPACE::fileno(file), 2);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(file), 0);
}

TEST_F(LlvmLibcFilenoTest, AStreamOverMemoryHasNone) {
  // A stream over memory was never given a descriptor by the system, so there
  // is none to report, and what follows the stream object is not one either.
  char storage[8] = {};
  ::FILE *memory = LIBC_NAMESPACE::fmemopen(storage, sizeof(storage), "r");
  ASSERT_TRUE(memory != nullptr);
  EXPECT_EQ(LIBC_NAMESPACE::fileno(memory), -1);
  ASSERT_ERRNO_EQ(EBADF);
  libc_errno = 0;
  ASSERT_EQ(LIBC_NAMESPACE::fclose(memory), 0);
}

TEST_F(LlvmLibcFilenoTest, AStreamThatGrowsItsOwnBufferHasNone) {
  char *buffer = nullptr;
  size_t length = 0;
  ::FILE *stream = LIBC_NAMESPACE::open_memstream(&buffer, &length);
  ASSERT_TRUE(stream != nullptr);
  EXPECT_EQ(LIBC_NAMESPACE::fileno(stream), -1);
  ASSERT_ERRNO_EQ(EBADF);
  libc_errno = 0;
  ASSERT_EQ(LIBC_NAMESPACE::fclose(stream), 0);
  ::free(buffer);
}
