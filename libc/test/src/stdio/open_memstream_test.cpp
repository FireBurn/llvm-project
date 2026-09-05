//===-- Unittests for open_memstream --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/func/free.h"
#include "hdr/types/FILE.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fflush.h"
#include "src/stdio/fwrite.h"
#include "src/stdio/open_memstream.h"
#include "src/string/memcmp.h"
#include "test/UnitTest/Test.h"

// What is written goes into a buffer the stream grows, and a flush tells the
// caller where that buffer is and how much of it has been written, with a
// terminator after it.
TEST(LlvmLibcOpenMemstreamTest, GrowsABufferAndReportsIt) {
  char *buffer = nullptr;
  size_t length = 0;
  ::FILE *stream = LIBC_NAMESPACE::open_memstream(&buffer, &length);
  ASSERT_FALSE(stream == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::fwrite("hello", 1, 5, stream), size_t(5));
  ASSERT_EQ(LIBC_NAMESPACE::fflush(stream), 0);
  ASSERT_EQ(length, size_t(5));
  ASSERT_FALSE(buffer == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::memcmp(buffer, "hello", 6), 0);

  // Far more than any first allocation, so the buffer has to grow.
  for (int i = 0; i < 1000; ++i)
    ASSERT_EQ(LIBC_NAMESPACE::fwrite("0123456789", 1, 10, stream), size_t(10));
  ASSERT_EQ(LIBC_NAMESPACE::fclose(stream), 0);
  ASSERT_EQ(length, size_t(10005));
  ASSERT_EQ(buffer[length], '\0');
  ASSERT_EQ(LIBC_NAMESPACE::memcmp(buffer + length - 10, "0123456789", 10), 0);
  ::free(buffer);
}
