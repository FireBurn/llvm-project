//===-- Unittests for the stdio_ext stream accessors ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/FILE.h"
#include "src/stdio/fclose.h"
#include "src/stdio/ferror.h"
#include "src/stdio/fflush.h"
#include "src/stdio/fgetc.h"
#include "src/stdio/fopen.h"
#include "src/stdio/fwrite.h"
#include "src/stdio/remove.h"
#include "src/stdio_ext/__fbufsize.h"
#include "src/stdio_ext/__flbf.h"
#include "src/stdio_ext/__fpending.h"
#include "src/stdio_ext/__fpurge.h"
#include "src/stdio_ext/__freadable.h"
#include "src/stdio_ext/__freadahead.h"
#include "src/stdio_ext/__freading.h"
#include "src/stdio_ext/__fseterr.h"
#include "src/stdio_ext/__fwritable.h"
#include "src/stdio_ext/__fwriting.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcStdioExtTest, WriteOnlyStream) {
  constexpr char TEXT[] = "0123456789";
  auto path = libc_make_test_file_path("stdio_ext_write.test");
  ::FILE *f = LIBC_NAMESPACE::fopen(path, "w");
  ASSERT_FALSE(f == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::__fwritable(f), 1);
  ASSERT_EQ(LIBC_NAMESPACE::__freadable(f), 0);
  ASSERT_GT(LIBC_NAMESPACE::__fbufsize(f), size_t(0));

  // Nothing has been written, so nothing is waiting to go out.
  ASSERT_EQ(LIBC_NAMESPACE::__fpending(f), size_t(0));

  ASSERT_EQ(LIBC_NAMESPACE::fwrite(TEXT, 1, sizeof(TEXT) - 1, f),
            sizeof(TEXT) - 1);
  ASSERT_EQ(LIBC_NAMESPACE::__fwriting(f), 1);
  ASSERT_EQ(LIBC_NAMESPACE::__freading(f), 0);
  ASSERT_EQ(LIBC_NAMESPACE::__fpending(f), sizeof(TEXT) - 1);

  // Throwing the buffer away leaves nothing pending, and does not write it.
  LIBC_NAMESPACE::__fpurge(f);
  ASSERT_EQ(LIBC_NAMESPACE::__fpending(f), size_t(0));

  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
  ASSERT_EQ(LIBC_NAMESPACE::remove(path), 0);
}

TEST(LlvmLibcStdioExtTest, ReadOnlyStream) {
  constexpr char TEXT[] = "abcdef";
  auto path = libc_make_test_file_path("stdio_ext_read.test");

  ::FILE *w = LIBC_NAMESPACE::fopen(path, "w");
  ASSERT_FALSE(w == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fwrite(TEXT, 1, sizeof(TEXT) - 1, w),
            sizeof(TEXT) - 1);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(w), 0);

  ::FILE *f = LIBC_NAMESPACE::fopen(path, "r");
  ASSERT_FALSE(f == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::__freadable(f), 1);
  ASSERT_EQ(LIBC_NAMESPACE::__fwritable(f), 0);

  // One character read fills the buffer, leaving the rest of the file
  // waiting in it.
  ASSERT_EQ(LIBC_NAMESPACE::fgetc(f), int('a'));
  ASSERT_EQ(LIBC_NAMESPACE::__freading(f), 1);
  ASSERT_EQ(LIBC_NAMESPACE::__fwriting(f), 0);
  ASSERT_EQ(LIBC_NAMESPACE::__freadahead(f), sizeof(TEXT) - 2);

  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
  ASSERT_EQ(LIBC_NAMESPACE::remove(path), 0);
}

TEST(LlvmLibcStdioExtTest, SetError) {
  auto path = libc_make_test_file_path("stdio_ext_error.test");
  ::FILE *f = LIBC_NAMESPACE::fopen(path, "w");
  ASSERT_FALSE(f == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::ferror(f), 0);
  LIBC_NAMESPACE::__fseterr(f);
  ASSERT_NE(LIBC_NAMESPACE::ferror(f), 0);

  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
  ASSERT_EQ(LIBC_NAMESPACE::remove(path), 0);
}

TEST(LlvmLibcStdioExtTest, LineBuffering) {
  auto path = libc_make_test_file_path("stdio_ext_lbf.test");
  ::FILE *f = LIBC_NAMESPACE::fopen(path, "w");
  ASSERT_FALSE(f == nullptr);

  // A file is fully buffered unless it is asked to be otherwise.
  ASSERT_EQ(LIBC_NAMESPACE::__flbf(f), 0);

  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
  ASSERT_EQ(LIBC_NAMESPACE::remove(path), 0);
}

TEST(LlvmLibcStdioExtTest, NullStream) {
  ASSERT_EQ(LIBC_NAMESPACE::__fbufsize(nullptr), size_t(0));
  ASSERT_EQ(LIBC_NAMESPACE::__flbf(nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::__fpending(nullptr), size_t(0));
  ASSERT_EQ(LIBC_NAMESPACE::__freadable(nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::__freadahead(nullptr), size_t(0));
  ASSERT_EQ(LIBC_NAMESPACE::__freading(nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::__fwritable(nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::__fwriting(nullptr), 0);
  LIBC_NAMESPACE::__fpurge(nullptr);
  LIBC_NAMESPACE::__fseterr(nullptr);
}
